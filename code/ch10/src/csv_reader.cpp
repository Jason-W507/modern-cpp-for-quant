#include "quant/ch10/csv_reader.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

namespace quant::ch10 {
namespace {

template <typename Number>
bool parse_number(const std::string& text, Number& value) {
  std::istringstream parser{text};
  if (!(parser >> value)) {
    return false;
  }
  std::string trailing;
  return !(parser >> trailing);
}

bool split_row(const std::string& row, std::array<std::string, 4>& fields) {
  if (std::count(row.begin(), row.end(), ',') != 3) {
    return false;
  }
  std::istringstream columns{row};
  return static_cast<bool>(std::getline(columns, fields[0], ',') &&
                           std::getline(columns, fields[1], ',') &&
                           std::getline(columns, fields[2], ',') &&
                           std::getline(columns, fields[3]));
}

ParseError error(std::size_t line, std::string field, std::string message) {
  return ParseError{line, std::move(field), std::move(message)};
}

}  // namespace

FillParseResult parse_fill_row(const std::string& row, std::size_t line) {
  std::array<std::string, 4> fields;
  if (!split_row(row, fields)) {
    return error(line, "row", "expected 4 columns");
  }

  Side side{Side::buy};
  if (fields[1] == "sell") {
    side = Side::sell;
  } else if (fields[1] != "buy") {
    return error(line, "side", "expected buy or sell");
  }

  std::int64_t quantity{};
  if (!parse_number(fields[2], quantity) || quantity <= 0) {
    return error(line, "quantity", "expected a positive integer");
  }
  double price{};
  if (!parse_number(fields[3], price) || price <= 0.0) {
    return error(line, "price", "expected a positive number");
  }

  try {
    return Fill{fields[0], side, quantity, price};
  } catch (const std::invalid_argument& exception) {
    return error(line, "fill", exception.what());
  }
}

}  // namespace quant::ch10
