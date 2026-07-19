#include "quant/ch08/event_reader.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace quant::ch08 {
namespace {

void strip_carriage_return(std::string& line) {
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }
}

std::vector<std::string> split_csv_row(const std::string& line) {
  std::vector<std::string> fields;
  std::size_t start = 0;
  while (true) {
    const std::size_t comma = line.find(',', start);
    if (comma == std::string::npos) {
      fields.push_back(line.substr(start));
      return fields;
    }
    fields.push_back(line.substr(start, comma - start));
    start = comma + 1;
  }
}

double parse_price(const std::string& text, std::size_t line_number) {
  std::size_t consumed = 0;
  try {
    const double value = std::stod(text, &consumed);
    if (consumed != text.size()) {
      throw std::runtime_error{"trailing characters"};
    }
    return value;
  } catch (const std::exception&) {
    throw std::runtime_error{"invalid price at line " +
                             std::to_string(line_number)};
  }
}

std::int64_t parse_quantity(const std::string& text,
                            std::size_t line_number) {
  std::size_t consumed = 0;
  try {
    const long long value = std::stoll(text, &consumed);
    if (consumed != text.size()) {
      throw std::runtime_error{"trailing characters"};
    }
    return static_cast<std::int64_t>(value);
  } catch (const std::exception&) {
    throw std::runtime_error{"invalid quantity at line " +
                             std::to_string(line_number)};
  }
}

}  // namespace

std::vector<MarketEvent> read_events(const std::string& path) {
  std::ifstream input{path};
  if (!input) {
    throw std::runtime_error{"cannot open event file: " + path};
  }

  std::string line;
  if (!std::getline(input, line)) {
    throw std::runtime_error{"event file is empty"};
  }
  strip_carriage_return(line);
  if (line != "symbol,price,quantity") {
    throw std::runtime_error{"expected header symbol,price,quantity"};
  }

  std::vector<MarketEvent> events;
  std::size_t line_number = 1;
  while (std::getline(input, line)) {
    ++line_number;
    strip_carriage_return(line);
    const auto fields = split_csv_row(line);
    if (fields.size() != 3) {
      throw std::runtime_error{"expected 3 fields at line " +
                               std::to_string(line_number)};
    }
    events.emplace_back(fields[0], parse_price(fields[1], line_number),
                        parse_quantity(fields[2], line_number));
  }
  if (events.empty()) {
    throw std::runtime_error{"event file contains no rows"};
  }
  return events;
}

}  // namespace quant::ch08
