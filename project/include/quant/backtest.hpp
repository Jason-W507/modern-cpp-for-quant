#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <optional>
#include <stdexcept>
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
  std::optional<PortfolioSnapshot> final_portfolio;
  PerformanceSummary performance;
};

struct BacktestConfig final {
  double initial_cash{};
  ExecutionConfig execution;
};

class BacktestEngine final {
 public:
  explicit BacktestEngine(BacktestConfig config) : config_(config) {
    if (!std::isfinite(config_.initial_cash) || config_.initial_cash <= 0.0) {
      throw std::invalid_argument{
          "backtest requires finite positive initial cash"};
    }
    validate_execution_config(config_.execution);
  }

  explicit BacktestEngine(double initial_cash)
      : BacktestEngine(
            BacktestConfig{.initial_cash = initial_cash, .execution = {}}) {}

  template <StrategyForMarketEvent Strategy>
  [[nodiscard]] BacktestResult run(const std::vector<MarketEvent>& events,
                                   const Strategy& strategy) const {
    Portfolio portfolio{config_.initial_cash};
    SimulatedExchange exchange{config_.execution};
    BacktestResult result;
    std::vector<double> equity_curve;
    equity_curve.reserve(events.size());

    if (events.empty()) {
      result.performance =
          summarize_performance(config_.initial_cash, equity_curve,
                                result.fills);
      return result;
    }

    const std::string& run_symbol = events.front().symbol();
    const bool has_mixed_symbols =
        std::any_of(events.begin(), events.end(), [&run_symbol](const auto& event) {
          return event.symbol() != run_symbol;
        });
    if (has_mixed_symbols) {
      throw std::invalid_argument{
          "single-symbol backtest requires one symbol; define a complete price "
          "map before enabling multi-asset valuation"};
    }
    const bool travels_backward = std::adjacent_find(
        events.begin(), events.end(), [](const auto& earlier, const auto& later) {
          return later.timestamp() < earlier.timestamp();
        }) != events.end();
    if (travels_backward) {
      throw std::invalid_argument{
          "single-symbol backtest requires nondecreasing timestamps; events "
          "with equal timestamps preserve input order"};
    }

    for (const MarketEvent& event : events) {
      const auto before = portfolio.snapshot(event.symbol(), event.price());
      if (const auto order = strategy.on_market_event(event, before)) {
        if (const auto fill = exchange.match(*order, event)) {
          portfolio.apply_fill(*fill);
          result.fills.push_back(*fill);
        }
      }
      result.final_portfolio = portfolio.snapshot(event.symbol(), event.price());
      equity_curve.push_back(result.final_portfolio->equity);
    }

    result.performance =
        summarize_performance(config_.initial_cash, equity_curve, result.fills);
    return result;
  }

 private:
  BacktestConfig config_;
};

}  // namespace quant
