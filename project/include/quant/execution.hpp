#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

#include "quant/types.hpp"

namespace quant {

class SimulatedExchange final {
 public:
  [[nodiscard]] std::optional<Fill> match(const OrderIntent& order,
                                          const MarketEvent& event) const {
    if (order.symbol != event.symbol || order.quantity <= 0 ||
        event.quantity < order.quantity || event.price <= 0.0) {
      return std::nullopt;
    }
    return Fill{event.timestamp, order.symbol, order.side, order.quantity,
                event.price};
  }
};

class Portfolio final {
 public:
  explicit Portfolio(double initial_cash) : cash_(initial_cash) {}

  void apply_fill(const Fill& fill) {
    const auto delta = signed_quantity(fill.side, fill.quantity);
    positions_[fill.symbol] += delta;
    cash_ -= static_cast<double>(delta) * fill.price;
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
