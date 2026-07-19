#ifndef QUANT_CH09_STRATEGY_RUNNER_HPP
#define QUANT_CH09_STRATEGY_RUNNER_HPP

#include <cstddef>
#include <concepts>
#include <optional>
#include <utility>
#include <vector>

#include "quant/ch09/domain.hpp"

namespace quant::ch09 {

template <typename Strategy>
concept MarketStrategy = requires(const Strategy& strategy,
                                  const MarketEvent& event,
                                  const PortfolioSnapshot& portfolio) {
  { strategy.on_event(event, portfolio) }
      -> std::same_as<std::optional<Order>>;
};

template <MarketStrategy Strategy>
class StrategyRunner final {
 public:
  explicit StrategyRunner(Strategy strategy) : strategy_(std::move(strategy)) {}

  std::size_t count_orders(
      const std::vector<MarketEvent>& events,
      const PortfolioSnapshot& portfolio) const {
    std::size_t count = 0;
    for (const MarketEvent& event : events) {
      if (strategy_.on_event(event, portfolio).has_value()) {
        ++count;
      }
    }
    return count;
  }

 private:
  Strategy strategy_;
};

}  // namespace quant::ch09

#endif
