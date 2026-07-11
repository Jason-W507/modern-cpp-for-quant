#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
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
  PerformanceSummary summary;
  summary.trades.fill_count = fills.size();
  for (const Fill& fill : fills) {
    if (fill.side == Side::buy) {
      summary.trades.buy_quantity += fill.quantity;
    } else {
      summary.trades.sell_quantity += fill.quantity;
    }
    summary.trades.gross_notional +=
        fill.price * static_cast<double>(fill.quantity);
  }

  if (equity_curve.empty()) {
    return summary;
  }
  if (initial_cash != 0.0) {
    summary.total_return = equity_curve.back() / initial_cash - 1.0;
  }

  double peak = initial_cash;
  std::vector<double> returns;
  returns.reserve(equity_curve.size() > 1 ? equity_curve.size() - 1 : 0);
  for (std::size_t index = 0; index < equity_curve.size(); ++index) {
    const double equity = equity_curve[index];
    peak = std::max(peak, equity);
    if (peak > 0.0) {
      summary.max_drawdown =
          std::max(summary.max_drawdown, (peak - equity) / peak);
    }
    if (index > 0 && equity_curve[index - 1] != 0.0) {
      returns.push_back(equity / equity_curve[index - 1] - 1.0);
    }
  }

  if (returns.size() >= 2) {
    double mean = 0.0;
    for (const double value : returns) {
      mean += value;
    }
    mean /= static_cast<double>(returns.size());
    double squared_deviation = 0.0;
    for (const double value : returns) {
      const double deviation = value - mean;
      squared_deviation += deviation * deviation;
    }
    summary.volatility =
        std::sqrt(squared_deviation / static_cast<double>(returns.size() - 1));
  }
  return summary;
}

}  // namespace quant
