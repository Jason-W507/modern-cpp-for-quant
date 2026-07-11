#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "quant/backtest.hpp"

namespace {
void require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}
bool close_to(double lhs, double rhs) { return std::abs(lhs - rhs) < 1e-9; }
}  // namespace

int main() {
  const std::vector<quant::MarketEvent> events{
      {{}, "AAPL", 99.0, 1'000},
      {{}, "AAPL", 101.0, 1'000},
      {{}, "AAPL", 103.0, 1'000},
  };
  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::BacktestEngine engine{10'000.0};

  const auto result = engine.run(events, strategy);
  require(result.fills.size() == 1, "strategy should fill exactly once");
  require(result.final_portfolio.quantity == 25,
          "final position should contain the purchased shares");
  require(close_to(result.final_portfolio.cash, 7'525.0),
          "final cash should reflect the purchase");
  require(close_to(result.final_portfolio.equity, 10'100.0),
          "final equity should use the last market price");
  require(close_to(result.total_return, 0.01),
          "total return should be one percent");
  require(close_to(result.max_drawdown, 0.0),
          "monotone equity should have zero drawdown");

  std::cout << "end-to-end backtest seam ok\n";
}
