#pragma once

#include <cstdint>
#include <cmath>
#include <optional>
#include <stdexcept>

#include "quant/types.hpp"

namespace quant {

class ThresholdStrategy final {
 public:
  ThresholdStrategy(double buy_below_or_equal, std::int64_t order_quantity)
      : threshold_(buy_below_or_equal), order_quantity_(order_quantity) {
    if (!std::isfinite(threshold_) || threshold_ <= 0.0) {
      throw std::invalid_argument{
          "strategy threshold must be finite and positive"};
    }
    if (order_quantity_ <= 0) {
      throw std::invalid_argument{"strategy order quantity must be positive"};
    }
  }

  [[nodiscard]] std::optional<OrderIntent> on_market_event(
      const MarketEvent& event, const PortfolioSnapshot& portfolio) const {
    if (event.symbol() != portfolio.symbol) {
      throw std::invalid_argument{
          "strategy event and portfolio symbols must match"};
    }
    if (event.price() > threshold_ || portfolio.quantity != 0) {
      return std::nullopt;
    }
    return OrderIntent{event.symbol(), Side::buy, order_quantity_};
  }

 private:
  double threshold_;
  std::int64_t order_quantity_;
};

}  // namespace quant
