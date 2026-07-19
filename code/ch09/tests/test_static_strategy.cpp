#include <iostream>
#include <vector>

#include "quant/ch09/strategy_runner.hpp"
#include "quant/ch09/strategies.hpp"
#include "test_support.hpp"

int main() {
  const quant::ch09::MarketEvent cheap_event{"AAPL", 99.0, 1'000};
  const std::vector<quant::ch09::MarketEvent> cheap{cheap_event};
  const quant::ch09::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};

  const quant::ch09::ThresholdStrategy threshold_strategy{100.0, 25};
  const auto order = threshold_strategy.on_event(cheap_event, flat);
  test_support::require(order.has_value(),
                        "threshold strategy should create an order");
  test_support::require(order->symbol() == "AAPL" && order->quantity() == 25,
                        "threshold strategy should preserve order details");

  const quant::ch09::StrategyRunner<quant::ch09::ThresholdStrategy> threshold{
      threshold_strategy};
  test_support::require(threshold.count_orders(cheap, flat) == 1,
                        "threshold strategy should create an order");

  const quant::ch09::StrategyRunner<quant::ch09::HoldStrategy> hold{
      quant::ch09::HoldStrategy{}};
  test_support::require(hold.count_orders(cheap, flat) == 0,
                        "hold strategy should create no order");

  std::cout << "static strategy seam ok\n";
}
