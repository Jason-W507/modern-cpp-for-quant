#include <iostream>
#include <optional>
#include <string>

#include "quant/ch09/strategies.hpp"

template <typename Strategy>
std::optional<quant::ch09::Order> decide(
    const Strategy& strategy, const quant::ch09::MarketEvent& event,
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
  const quant::ch09::MarketEvent event{"AAPL", 99.0, 1'000};
  const quant::ch09::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};
  const quant::ch09::ThresholdStrategy threshold{100.0, 25};
  const quant::ch09::HoldStrategy hold;

  std::cout << "threshold=" << order_label(decide(threshold, event, flat))
            << " hold=" << order_label(decide(hold, event, flat))
            << '\n';
}
