#pragma once

#include <algorithm>
#include <concepts>
#include <optional>
#include <vector>

#include "quant/execution.hpp"
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
  double total_return{};
  double max_drawdown{};
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
    double peak_equity = initial_cash_;

    if (events.empty()) {
      result.final_portfolio = PortfolioSnapshot{"", 0, initial_cash_,
                                                  initial_cash_};
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
      peak_equity = std::max(peak_equity, result.final_portfolio.equity);
      if (peak_equity > 0.0) {
        result.max_drawdown =
            std::max(result.max_drawdown,
                     (peak_equity - result.final_portfolio.equity) /
                         peak_equity);
      }
    }

    if (initial_cash_ != 0.0) {
      result.total_return =
          result.final_portfolio.equity / initial_cash_ - 1.0;
    }
    return result;
  }

 private:
  double initial_cash_;
};

}  // namespace quant
