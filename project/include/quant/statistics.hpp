#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "quant/types.hpp"

namespace quant {

struct TradeSummary final {
  std::size_t fill_count{};
  std::int64_t buy_quantity{};
  std::int64_t sell_quantity{};
  double gross_notional{};
};

struct PerformanceSummary final {
  double total_return{};
  double max_drawdown{};
  double volatility{};  // sample standard deviation of per-event returns
  TradeSummary trades;
};

inline PerformanceSummary summarize_performance(
    double initial_cash, const std::vector<double>& equity_curve,
    const std::vector<Fill>& fills) {
  if (!std::isfinite(initial_cash) || initial_cash <= 0.0) {
    throw std::invalid_argument{
        "performance requires finite positive initial cash"};
  }
  PerformanceSummary summary;
  summary.trades.fill_count = fills.size();
  double notional_compensation = 0.0;
  const auto checked_quantity_add = [](std::int64_t& total,
                                       std::int64_t quantity) {
    if (quantity > std::numeric_limits<std::int64_t>::max() - total) {
      throw std::overflow_error{"trade quantity total overflow"};
    }
    total += quantity;
  };
  for (const Fill& fill : fills) {
    if (fill.side() == Side::buy) {
      checked_quantity_add(summary.trades.buy_quantity, fill.quantity());
    } else {
      checked_quantity_add(summary.trades.sell_quantity, fill.quantity());
    }
    const double term =
        fill.price() * static_cast<double>(fill.quantity());
    if (!std::isfinite(term)) {
      throw std::overflow_error{"trade notional multiplication overflow"};
    }
    const double corrected = term - notional_compensation;
    const double next = summary.trades.gross_notional + corrected;
    if (!std::isfinite(next)) {
      throw std::overflow_error{"trade notional total overflow"};
    }
    notional_compensation =
        (next - summary.trades.gross_notional) - corrected;
    summary.trades.gross_notional = next;
  }

  if (equity_curve.empty()) {
    return summary;
  }
  summary.total_return = equity_curve.back() / initial_cash - 1.0;

  double peak = initial_cash;
  std::size_t return_count = 0;
  double return_mean = 0.0;
  double return_m2 = 0.0;
  double previous_equity = initial_cash;
  for (std::size_t index = 0; index < equity_curve.size(); ++index) {
    const double equity = equity_curve[index];
    if (!std::isfinite(equity) || equity <= 0.0) {
      throw std::invalid_argument{
          "performance equity marks must be finite and positive"};
    }
    peak = std::max(peak, equity);
    summary.max_drawdown =
        std::max(summary.max_drawdown, (peak - equity) / peak);
    const double event_return = equity / previous_equity - 1.0;
    if (!std::isfinite(event_return)) {
      throw std::overflow_error{"performance return overflow"};
    }
    ++return_count;
    const double delta = event_return - return_mean;
    return_mean += delta / static_cast<double>(return_count);
    const double delta_after_mean = event_return - return_mean;
    return_m2 += delta * delta_after_mean;
    if (!std::isfinite(return_m2)) {
      throw std::overflow_error{"performance variance overflow"};
    }
    previous_equity = equity;
  }

  if (return_count >= 2) {
    summary.volatility = std::sqrt(
        return_m2 / static_cast<double>(return_count - 1));
  }
  return summary;
}

}  // namespace quant
