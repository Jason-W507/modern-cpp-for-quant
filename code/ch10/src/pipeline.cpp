#include "quant/ch10/pipeline.hpp"

#include <cstddef>
#include <exception>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "quant/ch10/csv_reader.hpp"

namespace quant::ch10 {
namespace {

void remove_trailing_carriage_return(std::string& row) {
  if (!row.empty() && row.back() == '\r') {
    row.pop_back();
  }
}

void record_error(std::ostream& log, const ParseError& error) {
  log << "error line=" << error.line << " field=" << error.field
      << " message=" << error.message << '\n';
}

void record_portfolio_error(std::ostream& log,
                            ProcessingMetrics& metrics,
                            std::size_t line,
                            const std::exception& exception) {
  record_error(log, ParseError{line, "portfolio", exception.what()});
  ++metrics.rows_rejected;
  ++metrics.batches_rolled_back;
}

}  // namespace

ProcessingMetrics process_fill_csv(std::istream& input,
                                   std::ostream& log,
                                   Portfolio& portfolio) {
  ProcessingMetrics metrics;
  std::string row;
  if (!std::getline(input, row)) {
    throw std::runtime_error{"line 1: expected symbol,side,quantity,price"};
  }
  remove_trailing_carriage_return(row);
  if (row != "symbol,side,quantity,price") {
    throw std::runtime_error{"line 1: expected symbol,side,quantity,price"};
  }

  std::size_t line = 1;
  while (std::getline(input, row)) {
    ++line;
    ++metrics.rows_seen;
    remove_trailing_carriage_return(row);
    const FillParseResult result = parse_fill_row(row, line);
    if (std::holds_alternative<ParseError>(result)) {
      record_error(log, std::get<ParseError>(result));
      ++metrics.rows_rejected;
      continue;
    }

    try {
      portfolio.apply_batch({std::get<Fill>(result)});
      ++metrics.rows_accepted;
      ++metrics.batches_committed;
    } catch (const std::logic_error& exception) {
      record_portfolio_error(log, metrics, line, exception);
    } catch (const std::overflow_error& exception) {
      record_portfolio_error(log, metrics, line, exception);
    }
  }
  if (input.bad()) {
    throw std::runtime_error{"line " + std::to_string(line + 1) +
                             ": input read failure"};
  }
  return metrics;
}

}  // namespace quant::ch10
