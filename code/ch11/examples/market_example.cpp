#include <iostream>
#include <vector>

#include "quant/ch11/market_summary.hpp"

int main() {
  const std::vector<quant::ch11::MarketRow> rows{{99.0, 2}, {101.0, 2}};
  const auto summary = quant::ch11::summarize(rows);
  std::cout << "example-quantity=" << summary.quantity
            << " example-vwap=" << summary.vwap << '\n';
}
