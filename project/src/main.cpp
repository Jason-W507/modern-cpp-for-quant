#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "quant/backtest.hpp"
#include "quant/csv.hpp"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: quant_backtest <market-data.csv>\n";
    return 2;
  }

  std::ifstream input{argv[1]};
  if (!input) {
    std::cerr << "cannot open market data: " << argv[1] << '\n';
    return 2;
  }
  const auto parsed = quant::read_market_events(input);
  if (!parsed.has_value()) {
    std::cerr << parsed.error << '\n';
    return 2;
  }

  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::BacktestEngine engine{10'000.0};
  const auto result = engine.run(parsed.events, strategy);
  std::cout << "fills=" << result.fills.size() << " equity="
            << result.final_portfolio.equity << " return=" << std::fixed
            << std::setprecision(0) << result.total_return * 100.0 << "%\n";
}
