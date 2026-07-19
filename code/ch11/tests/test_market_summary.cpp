#include <cmath>
#include <iostream>
#include <vector>

#include "quant/ch11/market_summary.hpp"

int main() {
  const std::vector<quant::ch11::MarketRow> rows{
      {100.0, 10}, {101.0, 5}, {102.0, 10}};
  const auto summary = quant::ch11::summarize(rows);
  if (summary.rows != 3 || summary.quantity != 25 ||
      std::abs(summary.notional - 2525.0) > 1e-12 ||
      std::abs(summary.vwap - 101.0) > 1e-12) {
    return 1;
  }
  std::cout << "market-summary-ok\n";
}
