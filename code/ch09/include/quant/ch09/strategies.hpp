#ifndef QUANT_CH09_STRATEGIES_HPP
#define QUANT_CH09_STRATEGIES_HPP

#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>

#include "quant/ch09/domain.hpp"

namespace quant::ch09 {

class ThresholdStrategy final {
 public:
  ThresholdStrategy(double threshold, std::int64_t order_quantity)
      : threshold_(threshold), order_quantity_(order_quantity) {
    if (!std::isfinite(threshold_) || threshold_ <= 0.0 ||
        order_quantity_ <= 0) {
      throw std::invalid_argument{"invalid threshold strategy configuration"};
    }
  }

  std::optional<Order> on_event(
      const MarketEvent& event,
      const PortfolioSnapshot& portfolio) const {
    if (event.symbol() != portfolio.symbol) {
      throw std::logic_error{"event and portfolio symbols must match"};
    }
    if (event.price() > threshold_ || portfolio.quantity != 0) {
      return std::nullopt;
    }
    return Order{event.symbol(), Side::buy, order_quantity_};
  }

 private:
  double threshold_;
  std::int64_t order_quantity_;
};

class HoldStrategy final {
 public:
  std::optional<Order> on_event(
      const MarketEvent&,
      const PortfolioSnapshot&) const {
    return std::nullopt;
  }
};

}  // namespace quant::ch09

#endif
