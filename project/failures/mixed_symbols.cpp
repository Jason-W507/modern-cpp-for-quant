#include <iostream>
#include <stdexcept>
#include <vector>

#include "quant/backtest.hpp"

int main() {
  const std::vector<quant::MarketEvent> events{
      {{}, "AAPL", 99.0, 1'000},
      {{}, "MSFT", 420.0, 1'000},
  };
  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::BacktestEngine engine{10'000.0};

  try {
    (void)engine.run(events, strategy);
  } catch (const std::invalid_argument& error) {
    std::cerr << "model-boundary-error " << error.what() << '\n';
    return 2;
  }
  std::cerr << "failure-experiment did not reject mixed symbols\n";
  return 1;
}
