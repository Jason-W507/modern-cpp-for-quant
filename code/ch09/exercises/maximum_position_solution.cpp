#include <cstdint>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "quant/ch09/strategy_runner.hpp"

class MaximumPositionStrategy final {
 public:
  MaximumPositionStrategy(std::int64_t maximum, std::int64_t order_quantity)
      : maximum_(maximum), order_quantity_(order_quantity) {
    if (maximum_ < 0 || order_quantity_ <= 0 || order_quantity_ > maximum_) {
      throw std::invalid_argument{"invalid maximum-position configuration"};
    }
  }

  std::optional<quant::ch09::Order> on_event(
      const quant::ch09::MarketEvent& event,
      const quant::ch09::PortfolioSnapshot& portfolio) const {
    if (event.symbol() != portfolio.symbol) {
      throw std::logic_error{"event and portfolio symbols must match"};
    }
    if (portfolio.quantity < 0) {
      throw std::logic_error{"portfolio quantity must not be negative"};
    }
    if (portfolio.quantity > maximum_ - order_quantity_) {
      return std::nullopt;
    }
    return quant::ch09::Order{event.symbol(), quant::ch09::Side::buy,
                              order_quantity_};
  }

 private:
  std::int64_t maximum_;
  std::int64_t order_quantity_;
};

std::string order_label(std::size_t order_count) {
  if (order_count == 1) {
    return "buy";
  }
  return "none";
}

int main() {
  const quant::ch09::StrategyRunner<MaximumPositionStrategy> runner{
      MaximumPositionStrategy{50, 25}};
  const std::vector<quant::ch09::MarketEvent> events{
      {"AAPL", 99.0, 1'000}};
  const quant::ch09::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};
  const quant::ch09::PortfolioSnapshot full{"AAPL", 50, 5'000.0, 10'000.0};
  const quant::ch09::PortfolioSnapshot wrong_symbol{
      "MSFT", 0, 10'000.0, 10'000.0};
  bool mismatch_rejected = false;
  try {
    runner.count_orders(events, wrong_symbol);
  } catch (const std::logic_error&) {
    mismatch_rejected = true;
  }
  if (!mismatch_rejected) {
    return 1;
  }
  std::cout << "flat=" << order_label(runner.count_orders(events, flat))
            << " full=" << order_label(runner.count_orders(events, full))
            << '\n';
}
