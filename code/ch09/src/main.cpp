#include <iostream>
#include <vector>

#include "quant/ch09/strategy_runner.hpp"
#include "quant/ch09/strategies.hpp"

int main() {
  const std::vector<quant::ch09::MarketEvent> events{
      {"AAPL", 99.0, 1'000}, {"AAPL", 101.0, 800}};
  const quant::ch09::PortfolioSnapshot flat{
      "AAPL", 0, 10'000.0, 10'000.0};
  const quant::ch09::StrategyRunner<quant::ch09::ThresholdStrategy> threshold{
      quant::ch09::ThresholdStrategy{100.0, 25}};
  const quant::ch09::StrategyRunner<quant::ch09::HoldStrategy> hold{
      quant::ch09::HoldStrategy{}};

  std::cout << "threshold_orders=" << threshold.count_orders(events, flat)
            << " hold_orders=" << hold.count_orders(events, flat) << '\n';
}
