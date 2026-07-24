#include <iostream>
#include <limits>
#include <stdexcept>

#include "quant/strategy.hpp"
#include "test_support.hpp"

int main() {
  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::PortfolioSnapshot flat{"AAPL", 0, 10'000.0, 10'000.0};

  const quant::MarketEvent cheap{{}, "AAPL", 99.0, 1'000};
  const auto buy = strategy.on_market_event(cheap, flat);
  test_support::require(buy.has_value(), "cheap event should create an order");
  test_support::require(buy->symbol() == "AAPL", "order symbol should match event");
  test_support::require(buy->side() == quant::Side::buy, "order should be a buy");
  test_support::require(buy->quantity() == 25, "order should use configured quantity");

  const quant::MarketEvent expensive{{}, "AAPL", 101.0, 1'000};
  test_support::require(!strategy.on_market_event(expensive, flat).has_value(),
                        "expensive event should not create an order");

  const quant::PortfolioSnapshot already_long{"AAPL", 25, 7'525.0, 10'000.0};
  test_support::require(!strategy.on_market_event(cheap, already_long).has_value(),
                        "existing position should suppress another buy");

  for (const double threshold : {0.0, -1.0,
                                 std::numeric_limits<double>::quiet_NaN()}) {
    bool rejected = false;
    try {
      (void)quant::ThresholdStrategy{threshold, 25};
    } catch (const std::invalid_argument&) {
      rejected = true;
    }
    test_support::require(rejected,
                          "invalid strategy threshold should fail at startup");
  }
  bool quantity_rejected = false;
  try {
    (void)quant::ThresholdStrategy{100.0, 0};
  } catch (const std::invalid_argument&) {
    quantity_rejected = true;
  }
  test_support::require(quantity_rejected,
                        "invalid strategy quantity should fail at startup");

  bool symbol_mismatch_rejected = false;
  try {
    (void)strategy.on_market_event(
        quant::MarketEvent{{}, "MSFT", 99.0, 1'000}, flat);
  } catch (const std::invalid_argument&) {
    symbol_mismatch_rejected = true;
  }
  test_support::require(symbol_mismatch_rejected,
                        "strategy should reject a mismatched portfolio symbol");

  std::cout << "strategy seam ok\n";
}
