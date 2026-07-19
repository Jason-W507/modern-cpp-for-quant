#include <iomanip>
#include <iostream>
#include <vector>

#include "quant/ch11/market_summary.hpp"

#ifdef CH11_BUILDING_LIBRARY
#error "a PRIVATE compile definition leaked into the application target"
#endif

int main() {
  const std::vector<quant::ch11::MarketRow> rows{
      {100.0, 10}, {101.0, 5}, {102.0, 10}};
  const auto summary = quant::ch11::summarize(rows);

  std::cout << std::fixed << std::setprecision(2)
            << "rows=" << summary.rows
            << " quantity=" << summary.quantity
            << " notional=" << summary.notional
            << " vwap=" << summary.vwap << '\n';
}
