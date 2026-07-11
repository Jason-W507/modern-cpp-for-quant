#include <iostream>

#include "quant/execution.hpp"
#include "test_support.hpp"

int main() {
  quant::SimulatedExchange exchange;
  quant::Portfolio portfolio{10'000.0};

  const quant::MarketEvent first_tick{{}, "AAPL", 99.0, 1'000};
  const auto buy = exchange.match(
      quant::OrderIntent{"AAPL", quant::Side::buy, 25}, first_tick);
  test_support::require(buy.has_value(), "valid buy should fill");
  portfolio.apply_fill(*buy);

  auto snapshot = portfolio.snapshot("AAPL", first_tick.price);
  test_support::require(snapshot.quantity == 25, "buy should increase position");
  test_support::require(test_support::close_to(snapshot.cash, 7'525.0),
                        "buy should decrease cash");
  test_support::require(test_support::close_to(snapshot.equity, 10'000.0),
                        "equity should be conserved at fill price");

  const quant::MarketEvent second_tick{{}, "AAPL", 101.0, 1'000};
  const auto sell = exchange.match(
      quant::OrderIntent{"AAPL", quant::Side::sell, 10}, second_tick);
  test_support::require(sell.has_value(), "valid sell should fill");
  portfolio.apply_fill(*sell);

  snapshot = portfolio.snapshot("AAPL", second_tick.price);
  test_support::require(snapshot.quantity == 15, "sell should reduce position");
  test_support::require(test_support::close_to(snapshot.cash, 8'535.0),
                        "sell should increase cash");
  test_support::require(test_support::close_to(snapshot.equity, 10'050.0),
                        "equity should include marked unrealized pnl");

  const quant::MarketEvent wrong_symbol{{}, "MSFT", 420.0, 500};
  test_support::require(!exchange
               .match(quant::OrderIntent{"AAPL", quant::Side::buy, 1},
                      wrong_symbol)
               .has_value(),
          "symbol mismatch should not fill");

  std::cout << "execution and portfolio seam ok\n";
}
