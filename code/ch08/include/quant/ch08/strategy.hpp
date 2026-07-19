#ifndef QUANT_CH08_STRATEGY_HPP
#define QUANT_CH08_STRATEGY_HPP

#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>

#include "quant/ch08/domain.hpp"

namespace quant::ch08 {

class Strategy {
 public:
  virtual ~Strategy() = default;

  virtual std::optional<Order> on_event(
      const MarketEvent& event,
      const PortfolioSnapshot& portfolio) const = 0;
};

class ThresholdStrategy final : public Strategy {
 public:
  ThresholdStrategy(double threshold, std::int64_t order_quantity)
      : threshold_(threshold), order_quantity_(order_quantity) {
    if (!std::isfinite(threshold_) || threshold_ <= 0.0) {
      throw std::invalid_argument{"threshold must be positive and finite"};
    }
    if (order_quantity_ <= 0) {
      throw std::invalid_argument{"order quantity must be positive"};
    }
  }

  std::optional<Order> on_event(
      const MarketEvent& event,
      const PortfolioSnapshot& portfolio) const override {
    if (portfolio.symbol != event.symbol()) {
      throw std::logic_error{"portfolio snapshot symbol does not match event"};
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

}  // namespace quant::ch08

#endif
