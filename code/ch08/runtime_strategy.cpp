#include <iostream>
#include <optional>

#include "quant/ch08/strategy.hpp"

class HoldStrategy final : public quant::ch08::Strategy {
 public:
  std::optional<quant::ch08::Order> on_event(
      const quant::ch08::MarketEvent&,
      const quant::ch08::PortfolioSnapshot&) const override {
    return std::nullopt;
  }
};

bool creates_order(const quant::ch08::Strategy& strategy) {
  const quant::ch08::MarketEvent event{"AAPL", 99.0, 1'000};
  const quant::ch08::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};
  return strategy.on_event(event, flat).has_value();
}

int main() {
  const quant::ch08::ThresholdStrategy threshold{100.0, 25};
  const HoldStrategy hold;
  std::cout << "threshold=" << creates_order(threshold)
            << " hold=" << creates_order(hold) << '\n';
}
