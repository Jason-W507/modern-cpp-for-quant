#include <iostream>
#include <limits>
#include <stdexcept>

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

  auto snapshot = portfolio.snapshot("AAPL", first_tick.price());
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

  snapshot = portfolio.snapshot("AAPL", second_tick.price());
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

  quant::SimulatedExchange costly_exchange{
      quant::ExecutionConfig{.fixed_fee = 1.25, .slippage_bps = 10.0}};
  quant::Portfolio costly_portfolio{10'000.0};
  const quant::MarketEvent quoted_at_100{{}, "AAPL", 100.0, 1'000};
  const auto costly_buy = costly_exchange.match(
      quant::OrderIntent{"AAPL", quant::Side::buy, 10}, quoted_at_100);
  test_support::require(costly_buy.has_value(),
                        "configured buy should fill");
  test_support::require(test_support::close_to(costly_buy->price(), 100.10),
                        "buy slippage should worsen the execution price");
  test_support::require(test_support::close_to(costly_buy->fee(), 1.25),
                        "fill should preserve its fixed fee");
  costly_portfolio.apply_fill(*costly_buy);
  const auto costly_snapshot = costly_portfolio.snapshot("AAPL", 100.0);
  test_support::require(test_support::close_to(costly_snapshot.cash, 8'997.75),
                        "cash should include execution price and fee");
  test_support::require(test_support::close_to(costly_snapshot.equity, 9'997.75),
                        "equity should expose slippage and fee cost");

  bool invalid_cost_rejected = false;
  try {
    (void)quant::SimulatedExchange{
        quant::ExecutionConfig{.fixed_fee = -1.0, .slippage_bps = 0.0}};
  } catch (const std::invalid_argument&) {
    invalid_cost_rejected = true;
  }
  test_support::require(invalid_cost_rejected,
                        "negative execution costs should be rejected");

  bool invalid_event_rejected = false;
  try {
    (void)quant::MarketEvent{{}, "AAPL",
                             std::numeric_limits<double>::quiet_NaN(), 1};
  } catch (const std::invalid_argument&) {
    invalid_event_rejected = true;
  }
  test_support::require(invalid_event_rejected,
                        "non-finite market prices should be rejected");

  bool invalid_fill_rejected = false;
  try {
    (void)quant::Fill{{}, "AAPL", quant::Side::sell, 1, 100.0,
                      std::numeric_limits<double>::infinity()};
  } catch (const std::invalid_argument&) {
    invalid_fill_rejected = true;
  }
  test_support::require(invalid_fill_rejected,
                        "non-finite fill fees should be rejected");

  bool invalid_side_rejected = false;
  try {
    (void)quant::OrderIntent{"AAPL", static_cast<quant::Side>(42), 1};
  } catch (const std::invalid_argument&) {
    invalid_side_rejected = true;
  }
  test_support::require(
      invalid_side_rejected,
      "explicitly constructed invalid side values should be rejected");

  bool invalid_cash_rejected = false;
  try {
    (void)quant::Portfolio{0.0};
  } catch (const std::invalid_argument&) {
    invalid_cash_rejected = true;
  }
  test_support::require(invalid_cash_rejected,
                        "cash accounts require positive initial cash");

  quant::Portfolio constrained{100.0};
  bool overspend_rejected = false;
  try {
    constrained.apply_fill(
        quant::Fill{{}, "AAPL", quant::Side::buy, 2, 60.0, 0.0});
  } catch (const std::domain_error&) {
    overspend_rejected = true;
  }
  test_support::require(overspend_rejected,
                        "cash accounts should reject purchases above cash");

  bool oversell_rejected = false;
  try {
    constrained.apply_fill(
        quant::Fill{{}, "AAPL", quant::Side::sell, 1, 60.0, 0.0});
  } catch (const std::domain_error&) {
    oversell_rejected = true;
  }
  test_support::require(oversell_rejected,
                        "cash accounts should reject sales above holdings");

  bool invalid_mark_rejected = false;
  try {
    (void)constrained.snapshot(
        "AAPL", std::numeric_limits<double>::quiet_NaN());
  } catch (const std::invalid_argument&) {
    invalid_mark_rejected = true;
  }
  test_support::require(invalid_mark_rejected,
                        "portfolio marks should be finite and positive");

  std::cout << "execution and portfolio seam ok\n";
}
