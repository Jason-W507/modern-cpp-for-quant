#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
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
    if (order.symbol() != event.symbol() ||
        event.quantity() < order.quantity()) {
      return std::nullopt;
    }
    const double slippage_rate = config_.slippage_bps / 10'000.0;
    const double execution_price =
        event.price() *
        (order.side() == Side::buy ? 1.0 + slippage_rate
                                   : 1.0 - slippage_rate);
    return Fill{event.timestamp(), order.symbol(), order.side(), order.quantity(),
                execution_price, config_.fixed_fee};
  }

 private:
  ExecutionConfig config_;
};

class Portfolio final {
 public:
  explicit Portfolio(double initial_cash) : cash_(initial_cash) {
    if (!std::isfinite(cash_) || cash_ <= 0.0) {
      throw std::invalid_argument{
          "cash account requires finite positive initial cash"};
    }
  }

  void apply_fill(const Fill& fill) {
    const auto found = positions_.find(fill.symbol());
    const std::int64_t current =
        found == positions_.end() ? 0 : found->second;
    const double notional =
        fill.price() * static_cast<double>(fill.quantity());
    if (!std::isfinite(notional)) {
      throw std::overflow_error{"fill notional is not finite"};
    }

    std::int64_t next_position{};
    double next_cash{};
    if (fill.side() == Side::buy) {
      if (current > std::numeric_limits<std::int64_t>::max() -
                        fill.quantity()) {
        throw std::overflow_error{"position quantity overflow"};
      }
      const double required_cash = notional + fill.fee();
      if (!std::isfinite(required_cash)) {
        throw std::overflow_error{"purchase cost is not finite"};
      }
      if (required_cash > cash_) {
        throw std::domain_error{"cash account cannot buy above available cash"};
      }
      next_position = current + fill.quantity();
      next_cash = cash_ - required_cash;
    } else {
      if (fill.quantity() > current) {
        throw std::domain_error{"cash account cannot sell above holdings"};
      }
      next_position = current - fill.quantity();
      next_cash = cash_ + notional - fill.fee();
      if (!std::isfinite(next_cash)) {
        throw std::overflow_error{"sale proceeds are not finite"};
      }
      if (next_cash < 0.0) {
        throw std::domain_error{"cash account fee exceeds available proceeds"};
      }
    }
    positions_[fill.symbol()] = next_position;
    cash_ = next_cash;
  }

  [[nodiscard]] PortfolioSnapshot snapshot(const std::string& symbol,
                                           double mark_price) const {
    if (symbol.empty()) {
      throw std::invalid_argument{"portfolio snapshot requires a symbol"};
    }
    if (!std::isfinite(mark_price) || mark_price <= 0.0) {
      throw std::invalid_argument{
          "portfolio mark price must be finite and positive"};
    }
    const auto found = positions_.find(symbol);
    const std::int64_t quantity =
        found == positions_.end() ? 0 : found->second;
    const double equity =
        cash_ + static_cast<double>(quantity) * mark_price;
    if (!std::isfinite(equity)) {
      throw std::overflow_error{"portfolio equity is not finite"};
    }
    return PortfolioSnapshot{symbol, quantity, cash_, equity};
  }

 private:
  double cash_;
  std::unordered_map<std::string, std::int64_t> positions_;
};

}  // namespace quant
