#include <iostream>
#include <stdexcept>

#include "quant/ch08/execution.hpp"
#include "test_support.hpp"

int main() {
  quant::ch08::SimulatedExchange exchange;
  quant::ch08::Portfolio portfolio{10'000.0};

  const quant::ch08::MarketEvent first_tick{"AAPL", 99.0, 1'000};
  const auto buy = exchange.match(
      quant::ch08::Order{"AAPL", quant::ch08::Side::buy, 25}, first_tick);
  test_support::require(buy.has_value(), "valid buy should fill");
  portfolio.apply(*buy);

  auto snapshot = portfolio.snapshot("AAPL", 99.0);
  test_support::require(snapshot.quantity == 25,
                        "buy should increase position");
  test_support::require(test_support::close_to(snapshot.cash, 7'525.0),
                        "buy should decrease cash");
  test_support::require(test_support::close_to(snapshot.equity, 10'000.0),
                        "equity should be conserved at fill price");

  const quant::ch08::MarketEvent second_tick{"AAPL", 101.0, 1'000};
  const auto sell = exchange.match(
      quant::ch08::Order{"AAPL", quant::ch08::Side::sell, 10}, second_tick);
  test_support::require(sell.has_value(), "valid sell should fill");
  portfolio.apply(*sell);

  snapshot = portfolio.snapshot("AAPL", 101.0);
  test_support::require(snapshot.quantity == 15,
                        "sell should reduce position");
  test_support::require(test_support::close_to(snapshot.cash, 8'535.0),
                        "sell should increase cash");
  test_support::require(test_support::close_to(snapshot.equity, 10'050.0),
                        "equity should include marked pnl");

  const quant::ch08::MarketEvent wrong_symbol{"MSFT", 420.0, 500};
  test_support::require(
      !exchange
           .match(quant::ch08::Order{"AAPL", quant::ch08::Side::buy, 1},
                  wrong_symbol)
           .has_value(),
      "symbol mismatch should not fill");

  const quant::ch08::MarketEvent thin_market{"AAPL", 101.0, 5};
  test_support::require(
      !exchange
           .match(quant::ch08::Order{"AAPL", quant::ch08::Side::buy, 6},
                  thin_market)
           .has_value(),
      "order larger than event liquidity should not fill");

  const auto before_rejection = portfolio.snapshot("AAPL", 101.0);
  bool rejected = false;
  try {
    portfolio.apply(
        quant::ch08::Fill{"AAPL", quant::ch08::Side::sell, 20, 101.0});
  } catch (const std::logic_error&) {
    rejected = true;
  }
  const auto after_rejection = portfolio.snapshot("AAPL", 101.0);
  test_support::require(rejected, "portfolio should reject an oversell");
  test_support::require(after_rejection.quantity == before_rejection.quantity &&
                            test_support::close_to(after_rejection.cash,
                                                   before_rejection.cash),
                        "rejected fill should not change state");

  bool mixed_symbol_rejected = false;
  try {
    portfolio.apply(
        quant::ch08::Fill{"MSFT", quant::ch08::Side::buy, 1, 420.0});
  } catch (const std::logic_error&) {
    mixed_symbol_rejected = true;
  }
  test_support::require(mixed_symbol_rejected,
                        "chapter snapshot should reject a second symbol");

  std::cout << "execution and portfolio seam ok\n";
}
