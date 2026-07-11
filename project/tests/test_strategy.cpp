#include <iostream>
#include <stdexcept>

#include "quant/strategy.hpp"

namespace {
void require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}
}  // namespace

int main() {
  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};

  const quant::MarketEvent cheap{{}, "AAPL", 99.0, 1'000};
  const auto buy = strategy.on_market_event(cheap, flat);
  require(buy.has_value(), "cheap event should create an order");
  require(buy->symbol == "AAPL", "order symbol should match event");
  require(buy->side == quant::Side::buy, "order should be a buy");
  require(buy->quantity == 25, "order should use configured quantity");

  const quant::MarketEvent expensive{{}, "AAPL", 101.0, 1'000};
  require(!strategy.on_market_event(expensive, flat).has_value(),
          "expensive event should not create an order");

  const quant::PortfolioSnapshot already_long{"AAPL", 25, 7'525.0, 10'000.0};
  require(!strategy.on_market_event(cheap, already_long).has_value(),
          "existing position should suppress another buy");

  std::cout << "strategy seam ok\n";
}
