#pragma once

#include <algorithm>
#include <concepts>
#include <optional>
#include <vector>

#include "quant/execution.hpp"
#include "quant/statistics.hpp"
#include "quant/strategy.hpp"
#include "quant/types.hpp"

namespace quant {

template <typename Strategy>
concept StrategyForMarketEvent = requires(const Strategy& strategy,
                                          const MarketEvent& event,
                                          const PortfolioSnapshot& portfolio) {
  { strategy.on_market_event(event, portfolio) }
      -> std::same_as<std::optional<OrderIntent>>;
};

struct BacktestResult final {
  std::vector<Fill> fills;
  PortfolioSnapshot final_portfolio;
  PerformanceSummary performance;
};

class BacktestEngine final {
 public:
  explicit BacktestEngine(double initial_cash) : initial_cash_(initial_cash) {}

  template <StrategyForMarketEvent Strategy>
  [[nodiscard]] BacktestResult run(const std::vector<MarketEvent>& events,
                                   const Strategy& strategy) const {
    Portfolio portfolio{initial_cash_};
    SimulatedExchange exchange;
    BacktestResult result;
    std::vector<double> equity_curve;
    equity_curve.reserve(events.size());

    if (events.empty()) {
      result.final_portfolio = PortfolioSnapshot{"", 0, initial_cash_,
                                                  initial_cash_};
      result.performance =
          summarize_performance(initial_cash_, equity_curve, result.fills);
      return result;
    }

    for (const MarketEvent& event : events) {
      const auto before = portfolio.snapshot(event.symbol, event.price);
      if (const auto order = strategy.on_market_event(event, before)) {
        if (const auto fill = exchange.match(*order, event)) {
          portfolio.apply_fill(*fill);
          result.fills.push_back(*fill);
        }
      }
      result.final_portfolio = portfolio.snapshot(event.symbol, event.price);
      equity_curve.push_back(result.final_portfolio.equity);
    }

    result.performance =
        summarize_performance(initial_cash_, equity_curve, result.fills);
    return result;
  }

 private:
  double initial_cash_;
};

}  // namespace quant
