#pragma once

#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "quant/types.hpp"

namespace quant {

struct ExecutionConfig final {
  double fixed_fee{};
  double slippage_bps{};
};

inline void validate_execution_config(const ExecutionConfig& config) {
  if (!std::isfinite(config.fixed_fee) || config.fixed_fee < 0.0 ||
      !std::isfinite(config.slippage_bps) || config.slippage_bps < 0.0 ||
      config.slippage_bps >= 10'000.0) {
    throw std::invalid_argument{
        "execution config requires finite nonnegative fee and slippage "
        "below 10000 bps"};
  }
}

class SimulatedExchange final {
 public:
  explicit SimulatedExchange(ExecutionConfig config = {}) : config_(config) {
    validate_execution_config(config_);
  }

  [[nodiscard]] std::optional<Fill> match(const OrderIntent& order,
                                          const MarketEvent& event) const {
    if (order.symbol != event.symbol || order.quantity <= 0 ||
        event.quantity < order.quantity || event.price <= 0.0) {
      return std::nullopt;
    }
    const double slippage_rate = config_.slippage_bps / 10'000.0;
    const double execution_price =
        event.price *
        (order.side == Side::buy ? 1.0 + slippage_rate
                                 : 1.0 - slippage_rate);
    return Fill{event.timestamp, order.symbol, order.side, order.quantity,
                execution_price, config_.fixed_fee};
  }

 private:
  ExecutionConfig config_;
};

class Portfolio final {
 public:
  explicit Portfolio(double initial_cash) : cash_(initial_cash) {}

  void apply_fill(const Fill& fill) {
    const auto delta = signed_quantity(fill.side, fill.quantity);
    positions_[fill.symbol] += delta;
    cash_ -= static_cast<double>(delta) * fill.price + fill.fee;
  }

  [[nodiscard]] PortfolioSnapshot snapshot(const std::string& symbol,
                                           double mark_price) const {
    const auto found = positions_.find(symbol);
    const std::int64_t quantity =
        found == positions_.end() ? 0 : found->second;
    return PortfolioSnapshot{symbol, quantity, cash_,
                             cash_ + static_cast<double>(quantity) * mark_price};
  }

 private:
  double cash_;
  std::unordered_map<std::string, std::int64_t> positions_;
};

}  // namespace quant
