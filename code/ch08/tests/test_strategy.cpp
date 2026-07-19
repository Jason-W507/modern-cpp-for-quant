#include <iostream>
#include <stdexcept>

#include "quant/ch08/strategy.hpp"
#include "test_support.hpp"

int main() {
  const quant::ch08::ThresholdStrategy strategy{100.0, 25};
  const quant::ch08::PortfolioSnapshot flat{"AAPL", 0, 10'000.0,
                                                10'000.0};

  const quant::ch08::MarketEvent cheap{"AAPL", 99.0, 1'000};
  const auto buy = strategy.on_event(cheap, flat);
  test_support::require(buy.has_value(), "cheap event should create an order");
  test_support::require(buy->symbol() == "AAPL", "order should keep symbol");
  test_support::require(buy->side() == quant::ch08::Side::buy,
                        "order should be a buy");
  test_support::require(buy->quantity() == 25,
                        "order should use configured quantity");

  const quant::ch08::MarketEvent expensive{"AAPL", 101.0, 1'000};
  test_support::require(!strategy.on_event(expensive, flat).has_value(),
                        "expensive event should not create an order");

  const quant::ch08::PortfolioSnapshot already_long{"AAPL", 25, 7'525.0,
                                                        10'000.0};
  test_support::require(!strategy.on_event(cheap, already_long).has_value(),
                        "existing position should suppress another buy");

  bool mismatch_rejected = false;
  try {
    const quant::ch08::PortfolioSnapshot wrong_symbol{"MSFT", 25, 7'525.0,
                                                          10'000.0};
    strategy.on_event(cheap, wrong_symbol);
  } catch (const std::logic_error&) {
    mismatch_rejected = true;
  }
  test_support::require(mismatch_rejected,
                        "snapshot and event symbols should agree");

  std::cout << "strategy seam ok\n";
}
