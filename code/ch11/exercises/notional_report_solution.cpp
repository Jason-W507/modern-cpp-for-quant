#include <iomanip>
#include <iostream>
#include <vector>

#include "quant/ch11/market_summary.hpp"

int main() {
  const std::vector<quant::ch11::MarketRow> rows{{99.0, 1}, {101.0, 1}};
  const auto summary = quant::ch11::summarize(rows);
  std::cout << std::fixed << std::setprecision(2)
            << "exercise-notional=" << summary.notional << '\n';
}
