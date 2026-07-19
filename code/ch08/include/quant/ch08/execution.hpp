#ifndef QUANT_CH08_EXECUTION_HPP
#define QUANT_CH08_EXECUTION_HPP

#include <cmath>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

#include "quant/ch08/domain.hpp"

namespace quant::ch08 {

class SimulatedExchange final {
 public:
  std::optional<Fill> match(const Order& order,
                            const MarketEvent& event) const {
    if (order.symbol() != event.symbol() ||
        order.quantity() > event.quantity()) {
      return std::nullopt;
    }
    return Fill{order.symbol(), order.side(), order.quantity(), event.price()};
  }
};

class Portfolio final {
 public:
  explicit Portfolio(double initial_cash) : cash_(initial_cash) {
    if (!std::isfinite(cash_) || cash_ < 0.0) {
      throw std::invalid_argument{"initial cash must be finite and non-negative"};
    }
  }

  void apply(const Fill& fill) {
    const double notional = static_cast<double>(fill.quantity()) * fill.price();

    if (!symbol_.empty() && symbol_ != fill.symbol()) {
      throw std::logic_error{"portfolio supports one symbol per run"};
    }
    if (fill.side() == Side::sell && quantity_ < fill.quantity()) {
      throw std::logic_error{"sell quantity exceeds position"};
    }
    if (fill.side() == Side::buy && cash_ < notional) {
      throw std::logic_error{"buy notional exceeds cash"};
    }

    if (symbol_.empty()) {
      symbol_ = fill.symbol();
    }
    if (fill.side() == Side::buy) {
      quantity_ += fill.quantity();
      cash_ -= notional;
    } else {
      quantity_ -= fill.quantity();
      cash_ += notional;
    }
  }

  PortfolioSnapshot snapshot(const std::string& symbol,
                             double mark_price) const {
    if (symbol.empty()) {
      throw std::invalid_argument{"symbol must not be empty"};
    }
    if (!std::isfinite(mark_price) || mark_price <= 0.0) {
      throw std::invalid_argument{"mark price must be positive and finite"};
    }
    if (!symbol_.empty() && symbol_ != symbol) {
      throw std::logic_error{"snapshot symbol does not match portfolio"};
    }
    return PortfolioSnapshot{
        symbol, quantity_, cash_,
        cash_ + static_cast<double>(quantity_) * mark_price};
  }

 private:
  double cash_;
  std::string symbol_;
  std::int64_t quantity_{};
};

}  // namespace quant::ch08

#endif
