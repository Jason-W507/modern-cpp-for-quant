#include <iostream>
#include <string>
#include <variant>

#include "quant/ch10/csv_reader.hpp"
#include "test_support.hpp"

int main() {
  const auto valid = quant::ch10::parse_fill_row("AAPL,buy,10,100.0", 2);
  test_support::require(std::holds_alternative<quant::ch10::Fill>(valid),
                        "valid row should produce a fill");
  const auto& fill = std::get<quant::ch10::Fill>(valid);
  test_support::require(fill.symbol() == "AAPL" && fill.quantity() == 10,
                        "parsed fill should preserve row values");

  const auto invalid = quant::ch10::parse_fill_row("AAPL,buy,oops,100.0", 3);
  test_support::require(std::holds_alternative<quant::ch10::ParseError>(invalid),
                        "invalid quantity should produce an explicit error");
  const auto& error = std::get<quant::ch10::ParseError>(invalid);
  test_support::require(
      error.line == 3 && error.field == "quantity" &&
          error.message == "expected a positive integer",
      "parse error should preserve line, field, and diagnostic context");

  std::cout << "csv error context ok\n";
}
