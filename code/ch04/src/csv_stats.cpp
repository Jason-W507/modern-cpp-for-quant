#include "quant/ch04/csv_stats.hpp"

#include <array>
#include <cmath>
#include <sstream>

namespace quant::ch04 {
namespace {

bool split_row(const std::string& row, std::array<std::string, 3>& fields) {
  std::size_t start{0};
  for (std::size_t index{0}; index < fields.size(); ++index) {
    const std::size_t comma{row.find(',', start)};
    if (index + 1 == fields.size()) {
      if (comma != std::string::npos) {
        return false;
      }
      fields[index] = row.substr(start);
      return true;
    }
    if (comma == std::string::npos) {
      return false;
    }
    fields[index] = row.substr(start, comma - start);
    start = comma + 1;
  }
  return true;
}

void remove_trailing_carriage_return(std::string& row) {
  if (!row.empty() && row.back() == '\r') {
    row.pop_back();
  }
}

std::size_t column_count(const std::string& row) {
  std::size_t columns{1};
  for (const char character : row) {
    if (character == ',') {
      ++columns;
    }
  }
  return columns;
}

bool parse_price(const std::string& text, double& value) {
  std::istringstream parser{text};
  if (!(parser >> value)) {
    return false;
  }
  std::string trailing;
  return !(parser >> trailing) && std::isfinite(value) && value > 0.0;
}

bool parse_quantity(const std::string& text, int& value) {
  std::istringstream parser{text};
  if (!(parser >> value)) {
    return false;
  }
  std::string trailing;
  return !(parser >> trailing) && value > 0;
}

void reject(std::vector<std::string>& symbols, std::vector<double>& prices,
            std::vector<int>& quantities, std::string& error,
            const std::size_t line, const std::string& reason) {
  symbols.clear();
  prices.clear();
  quantities.clear();
  error = "line " + std::to_string(line) + ": " + reason;
}

}  // namespace

bool read_market_csv(std::istream& input, std::vector<std::string>& symbols,
                     std::vector<double>& prices,
                     std::vector<int>& quantities, std::string& error) {
  symbols.clear();
  prices.clear();
  quantities.clear();
  error.clear();

  std::string row;
  if (!std::getline(input, row)) {
    reject(symbols, prices, quantities, error, 1,
           "expected symbol,price,quantity");
    return false;
  }
  remove_trailing_carriage_return(row);
  if (row != "symbol,price,quantity") {
    reject(symbols, prices, quantities, error, 1,
           "expected symbol,price,quantity");
    return false;
  }

  std::size_t line{1};
  while (std::getline(input, row)) {
    ++line;
    remove_trailing_carriage_return(row);
    std::array<std::string, 3> fields{};
    if (!split_row(row, fields)) {
      reject(symbols, prices, quantities, error, line,
             "expected 3 columns, observed " +
                 std::to_string(column_count(row)));
      return false;
    }
    if (fields[0].empty()) {
      reject(symbols, prices, quantities, error, line,
             "empty field: symbol");
      return false;
    }

    double price{0.0};
    int quantity{0};
    if (!parse_price(fields[1], price)) {
      reject(symbols, prices, quantities, error, line,
             "invalid number: price");
      return false;
    }
    if (!parse_quantity(fields[2], quantity)) {
      reject(symbols, prices, quantities, error, line,
             "invalid number: quantity");
      return false;
    }

    symbols.push_back(fields[0]);
    prices.push_back(price);
    quantities.push_back(quantity);
  }
  if (input.bad()) {
    reject(symbols, prices, quantities, error, line + 1,
           "input read failure");
    return false;
  }
  return true;
}

int total_quantity(const std::vector<int>& quantities) {
  int total{0};
  for (const int quantity : quantities) {
    total += quantity;
  }
  return total;
}

bool total_notional(const std::vector<double>& prices,
                    const std::vector<int>& quantities, double& total) {
  total = 0.0;
  if (prices.size() != quantities.size()) {
    return false;
  }
  for (std::size_t index{0}; index < prices.size(); ++index) {
    total += prices[index] * quantities[index];
  }
  return true;
}

}  // namespace quant::ch04
