#include "quant/ch07/csv_reader.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

namespace quant::ch07 {
namespace {

void remove_carriage_return(std::string& row) {
  if (!row.empty() && row.back() == '\r') {
    row.pop_back();
  }
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

bool parse_quantity(const std::string& text, int& quantity) {
  std::istringstream parser{text};
  std::string trailing;
  return static_cast<bool>(parser >> quantity) && !(parser >> trailing);
}

bool parse_price(const std::string& text, double& price) {
  std::istringstream parser{text};
  std::string trailing;
  return static_cast<bool>(parser >> price) && !(parser >> trailing);
}

Side parse_side(const std::string& text, const std::size_t line) {
  if (text == "buy") {
    return Side::buy;
  }
  if (text == "sell") {
    return Side::sell;
  }
  throw std::runtime_error{"line " + std::to_string(line) +
                           ": side must be buy or sell"};
}

}  // namespace

std::vector<Trade> read_trades(std::istream& input) {
  std::string row;
  if (!std::getline(input, row)) {
    throw std::runtime_error{"line 1: expected symbol,side,quantity,price"};
  }
  remove_carriage_return(row);
  if (row != "symbol,side,quantity,price") {
    throw std::runtime_error{"line 1: expected symbol,side,quantity,price"};
  }

  std::vector<Trade> trades;
  std::size_t line{1};
  while (std::getline(input, row)) {
    ++line;
    remove_carriage_return(row);
    std::array<std::string, 4> fields;
    if (!split_row(row, fields)) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": expected 4 columns"};
    }
    if (fields[0].empty()) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": symbol must not be empty"};
    }
    int quantity{};
    if (!parse_quantity(fields[2], quantity) || quantity <= 0) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": quantity must be a positive integer"};
    }
    double price{};
    if (!parse_price(fields[3], price) || !std::isfinite(price) ||
        price <= 0.0) {
      throw std::runtime_error{"line " + std::to_string(line) +
                               ": price must be finite and positive"};
    }
    trades.push_back(
        Trade{fields[0], parse_side(fields[1], line), quantity, price});
  }
  if (input.bad()) {
    throw std::runtime_error{"line " + std::to_string(line + 1) +
                             ": input read failure"};
  }
  return trades;
}

}  // namespace quant::ch07
