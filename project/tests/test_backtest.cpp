#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "quant/backtest.hpp"
#include "test_support.hpp"

int main() {
  const std::vector<quant::MarketEvent> events{
      {{}, "AAPL", 99.0, 1'000},
      {{}, "AAPL", 101.0, 1'000},
      {{}, "AAPL", 103.0, 1'000},
  };
  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::BacktestEngine engine{10'000.0};

  const auto result = engine.run(events, strategy);
  test_support::require(result.fills.size() == 1,
                        "strategy should fill exactly once");
  test_support::require(result.final_portfolio.has_value(),
                        "nonempty runs should produce a final portfolio");
  test_support::require(result.final_portfolio->quantity == 25,
                        "final position should contain the purchased shares");
  test_support::require(
      test_support::close_to(result.final_portfolio->cash, 7'525.0),
      "final cash should reflect the purchase");
  test_support::require(
      test_support::close_to(result.final_portfolio->equity, 10'100.0),
      "final equity should use the last market price");
  test_support::require(
      test_support::close_to(result.performance.total_return, 0.01),
      "total return should be one percent");
  test_support::require(
      test_support::close_to(result.performance.max_drawdown, 0.0),
      "monotone equity should have zero drawdown");
  test_support::require(
      test_support::close_to(result.performance.volatility,
                             0.002879597233822328, 1e-12),
      "volatility should include the initial-to-first-event return");
  test_support::require(result.performance.trades.fill_count == 1 &&
                            result.performance.trades.buy_quantity == 25,
                        "trade summary should aggregate the fill");

  const quant::BacktestConfig costly_config{
      .initial_cash = 10'000.0,
      .execution = {.fixed_fee = 1.0, .slippage_bps = 10.0}};
  const quant::BacktestEngine costly_engine{costly_config};
  const auto costly_result = costly_engine.run(events, strategy);
  test_support::require(costly_result.fills.size() == 1,
                        "cost model should preserve the order lifecycle");
  test_support::require(
      test_support::close_to(costly_result.fills.front().price(), 99.099),
      "configured slippage should change the execution price");
  test_support::require(
      test_support::close_to(costly_result.final_portfolio->cash, 7'521.525),
      "configured costs should flow through the cash ledger");
  test_support::require(
      test_support::close_to(costly_result.final_portfolio->equity, 10'096.525),
      "configured costs should flow through final equity");
  test_support::require(
      test_support::close_to(costly_result.performance.total_return, 0.0096525),
      "performance should use the cost-adjusted equity curve");

  const std::vector<quant::MarketEvent> mixed_symbols{
      {{}, "AAPL", 99.0, 1'000},
      {{}, "MSFT", 420.0, 1'000},
  };
  bool mixed_symbols_rejected = false;
  try {
    (void)engine.run(mixed_symbols, strategy);
  } catch (const std::invalid_argument& error) {
    mixed_symbols_rejected =
        std::string{error.what()}.find("single-symbol") != std::string::npos;
  }
  test_support::require(
      mixed_symbols_rejected,
      "mixed symbols should be rejected until a complete price map exists");

  const auto t0 = quant::Timestamp{};
  const auto t1 = t0 + std::chrono::milliseconds{1};
  bool reversed_time_rejected = false;
  try {
    (void)engine.run(
        std::vector<quant::MarketEvent>{{t1, "AAPL", 99.0, 1},
                                        {t0, "AAPL", 100.0, 1}},
        strategy);
  } catch (const std::invalid_argument& error) {
    reversed_time_rejected =
        std::string{error.what()}.find("nondecreasing") != std::string::npos;
  }
  test_support::require(
      reversed_time_rejected,
      "backtests should reject events that travel backward in time");

  const auto empty_result = engine.run({}, strategy);
  test_support::require(!empty_result.final_portfolio.has_value(),
                        "empty runs should not fabricate a symbol snapshot");

  const auto first_event_sensitive = quant::summarize_performance(
      100.0, std::vector<double>{90.0, 99.0}, {});
  test_support::require(
      test_support::close_to(first_event_sensitive.volatility,
                             0.1414213562373095, 1e-12),
      "volatility should include initial cash to first event return");

  bool zero_initial_cash_rejected = false;
  try {
    (void)quant::BacktestEngine{0.0};
  } catch (const std::invalid_argument&) {
    zero_initial_cash_rejected = true;
  }
  test_support::require(zero_initial_cash_rejected,
                        "undefined zero-capital returns should fail at startup");

  bool quantity_overflow_rejected = false;
  try {
    (void)quant::summarize_performance(
        100.0, {},
        {quant::Fill{{}, "AAPL", quant::Side::buy,
                     std::numeric_limits<std::int64_t>::max(), 1.0, 0.0},
         quant::Fill{{}, "AAPL", quant::Side::buy, 1, 1.0, 0.0}});
  } catch (const std::overflow_error&) {
    quantity_overflow_rejected = true;
  }
  test_support::require(quantity_overflow_rejected,
                        "trade quantity aggregation should reject overflow");

  bool notional_overflow_rejected = false;
  try {
    (void)quant::summarize_performance(
        100.0, {},
        {quant::Fill{{}, "AAPL", quant::Side::buy, 2,
                     std::numeric_limits<double>::max(), 0.0}});
  } catch (const std::overflow_error&) {
    notional_overflow_rejected = true;
  }
  test_support::require(notional_overflow_rejected,
                        "trade notional aggregation should reject infinity");

  bool return_overflow_rejected = false;
  try {
    (void)quant::summarize_performance(
        std::numeric_limits<double>::min(),
        {std::numeric_limits<double>::max()}, {});
  } catch (const std::overflow_error&) {
    return_overflow_rejected = true;
  }
  test_support::require(return_overflow_rejected,
                        "performance aggregation should reject infinite returns");

  std::cout << "end-to-end backtest seam ok\n";
}
