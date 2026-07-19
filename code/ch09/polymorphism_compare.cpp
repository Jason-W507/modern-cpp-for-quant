#include <iostream>
#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "quant/ch09/strategy_runner.hpp"
#include "quant/ch09/strategies.hpp"

class RuntimeStrategy {
 public:
  virtual ~RuntimeStrategy() = default;
  virtual std::optional<quant::ch09::Order> on_event(
      const quant::ch09::MarketEvent& event,
      const quant::ch09::PortfolioSnapshot& portfolio) const = 0;
};

class RuntimeThreshold final : public RuntimeStrategy {
 public:
  RuntimeThreshold(double threshold, std::int64_t quantity)
      : strategy_(threshold, quantity) {}

  std::optional<quant::ch09::Order> on_event(
      const quant::ch09::MarketEvent& event,
      const quant::ch09::PortfolioSnapshot& portfolio) const override {
    return strategy_.on_event(event, portfolio);
  }

 private:
  quant::ch09::ThresholdStrategy strategy_;
};

std::string order_label(const std::optional<quant::ch09::Order>& order) {
  if (order.has_value()) {
    return "buy";
  }
  return "none";
}

std::string count_label(std::size_t order_count) {
  if (order_count == 1) {
    return "buy";
  }
  return "none";
}

int main() {
  const quant::ch09::MarketEvent event{"AAPL", 99.0, 1'000};
  const std::vector<quant::ch09::MarketEvent> events{event};
  const quant::ch09::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};
  const quant::ch09::StrategyRunner<quant::ch09::ThresholdStrategy> static_path{
      quant::ch09::ThresholdStrategy{100.0, 25}};
  const RuntimeThreshold runtime_object{100.0, 25};
  const RuntimeStrategy& dynamic_path = runtime_object;

  std::cout << "static=" << count_label(static_path.count_orders(events, flat))
            << " dynamic=" << order_label(dynamic_path.on_event(event, flat))
            << '\n';
}
