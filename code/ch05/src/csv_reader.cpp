#include "quant/ch05/csv_reader.hpp"

#include <algorithm>
#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

namespace quant::ch05 {
namespace {

bool parse_price(const std::string& text, double& price) {
  std::istringstream parser{text};
  if (!(parser >> price)) {
    return false;
  }
  std::string trailing;
  return !(parser >> trailing);
}

bool parse_quantity(const std::string& text, int& quantity) {
  std::istringstream parser{text};
  if (!(parser >> quantity)) {
    return false;
  }
  std::string trailing;
  return !(parser >> trailing);
}

void remove_trailing_carriage_return(std::string& row) {
  if (!row.empty() && row.back() == '\r') {
    row.pop_back();
  }
}

bool split_row(const std::string& row, std::array<std::string, 3>& fields) {
  if (std::count(row.begin(), row.end(), ',') != 2) {
    return false;
  }
  std::istringstream columns{row};
  return static_cast<bool>(std::getline(columns, fields[0], ',') &&
                           std::getline(columns, fields[1], ',') &&
                           std::getline(columns, fields[2]));
}

}  // namespace

std::vector<MarketQuote> read_market_csv(std::istream& input) {
  std::vector<MarketQuote> quotes;
  std::string row;
  if (!std::getline(input, row)) {
    throw std::runtime_error{"line 1: expected symbol,price,quantity"};
  }
  remove_trailing_carriage_return(row);
  if (row != "symbol,price,quantity") {
    throw std::runtime_error{"line 1: expected symbol,price,quantity"};
  }
  std::size_t line{1};
  while (std::getline(input, row)) {
    ++line;
    remove_trailing_carriage_return(row);
    std::array<std::string, 3> fields;
    if (!split_row(row, fields)) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": expected 3 columns"};
    }
    double price{0.0};
    if (!parse_price(fields[1], price)) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": invalid number: price"};
    }
    int quantity{0};
    if (!parse_quantity(fields[2], quantity)) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": invalid number: quantity"};
    }
    try {
      quotes.emplace_back(fields[0], price, quantity);
    } catch (const std::exception& error) {
      throw std::runtime_error{"line " + std::to_string(line) + ": " +
                               error.what()};
    }
  }
  if (input.bad()) {
    throw std::runtime_error{"line " + std::to_string(line + 1) +
                             ": input read failure"};
  }
  return quotes;
}

}  // namespace quant::ch05
