#include <cassert>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "quant/ch10/csv_reader.hpp"

std::optional<double> latest_price(const std::vector<quant::ch10::Fill>& fills,
                                   const std::string& symbol) {
  assert(!symbol.empty());
  for (auto position = fills.rbegin(); position != fills.rend(); ++position) {
    if (position->symbol() == symbol) {
      return position->price();
    }
  }
  return std::nullopt;
}

void require_readable_file(const std::string& path) {
  std::ifstream input{path};
  if (!input) {
    throw std::runtime_error{"cannot open input file"};
  }
}

std::string optional_label(const std::optional<double>& value) {
  if (value.has_value()) {
    return "value";
  }
  return "none";
}

std::string exception_label(bool caught) {
  if (caught) {
    return "caught";
  }
  return "missed";
}

int main() {
  const std::vector<quant::ch10::Fill> fills{
      {"AAPL", quant::ch10::Side::buy, 10, 100.0}};
  const auto missing = latest_price(fills, "MSFT");
  const auto malformed =
      quant::ch10::parse_fill_row("AAPL,buy,oops,100.0", 7);
  std::string error_field{"missing"};
  const auto* parse_error =
      std::get_if<quant::ch10::ParseError>(&malformed);
  if (parse_error != nullptr) {
    error_field = parse_error->field;
  }

  bool external_caught = false;
  try {
    require_readable_file("definitely-missing.csv");
  } catch (const std::runtime_error&) {
    external_caught = true;
  }

  std::cout << "optional=" << optional_label(missing)
            << " explicit=" << error_field
            << " exception=" << exception_label(external_caught)
            << " assertion=internal-only\n";
}
