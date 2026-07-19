#include <iostream>
#include <optional>
#include <string>

#include "quant/ch09/strategies.hpp"

std::optional<quant::ch09::Order> decide_threshold(
    const quant::ch09::ThresholdStrategy& strategy,
    const quant::ch09::MarketEvent& event,
    const quant::ch09::PortfolioSnapshot& portfolio) {
  return strategy.on_event(event, portfolio);
}

std::string order_label(const std::optional<quant::ch09::Order>& order) {
  if (order.has_value()) {
    return "buy";
  }
  return "none";
}

int main() {
  const quant::ch09::ThresholdStrategy strategy{100.0, 25};
  const quant::ch09::MarketEvent event{"AAPL", 99.0, 1'000};
  const quant::ch09::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};
  const auto order = decide_threshold(strategy, event, flat);
  std::cout << "ordinary=" << order_label(order) << '\n';
}
