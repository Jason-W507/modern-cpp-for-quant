#include <iostream>
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
  test_support::require(result.final_portfolio.quantity == 25,
                        "final position should contain the purchased shares");
  test_support::require(
      test_support::close_to(result.final_portfolio.cash, 7'525.0),
      "final cash should reflect the purchase");
  test_support::require(
      test_support::close_to(result.final_portfolio.equity, 10'100.0),
      "final equity should use the last market price");
  test_support::require(
      test_support::close_to(result.performance.total_return, 0.01),
      "total return should be one percent");
  test_support::require(
      test_support::close_to(result.performance.max_drawdown, 0.0),
      "monotone equity should have zero drawdown");
  test_support::require(result.performance.volatility > 0.0 &&
                            result.performance.volatility < 0.001,
                        "step-return volatility should be small and positive");
  test_support::require(result.performance.trades.fill_count == 1 &&
                            result.performance.trades.buy_quantity == 25,
                        "trade summary should aggregate the fill");

  std::cout << "end-to-end backtest seam ok\n";
}
