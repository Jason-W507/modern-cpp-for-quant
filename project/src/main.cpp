#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "quant/backtest.hpp"
#include "quant/csv.hpp"

namespace {

bool parse_number(const std::string_view text, double& value) {
  const char* begin = text.data();
  const char* end = begin + text.size();
  const auto [position, error] = std::from_chars(begin, end, value);
  return error == std::errc{} && position == end;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2 && argc != 4) {
    std::cerr << "usage: quant_backtest <market-data.csv> "
                 "[fixed-fee slippage-bps]\n";
    return 2;
  }

  quant::ExecutionConfig execution;
  if (argc == 4 &&
      (!parse_number(argv[2], execution.fixed_fee) ||
       !parse_number(argv[3], execution.slippage_bps))) {
    std::cerr << "invalid execution config: expected two numbers\n";
    return 2;
  }
  try {
    quant::validate_execution_config(execution);
  } catch (const std::invalid_argument& error) {
    std::cerr << "invalid execution config: " << error.what() << '\n';
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
  const quant::BacktestEngine engine{quant::BacktestConfig{
      .initial_cash = 10'000.0, .execution = execution}};
  quant::BacktestResult result;
  try {
    result = engine.run(parsed.events, strategy);
  } catch (const std::invalid_argument& error) {
    std::cerr << "model-boundary-error " << error.what() << '\n';
    return 2;
  }
  std::cout << std::fixed << "backtest-ok events=" << parsed.events.size()
            << " fills=" << result.fills.size() << " cash="
            << std::setprecision(3) << result.final_portfolio.cash
            << " equity=" << result.final_portfolio.equity << " return="
            << std::setprecision(6) << result.performance.total_return * 100.0
            << "% max-drawdown=" << result.performance.max_drawdown * 100.0
            << "% volatility=" << result.performance.volatility
            << " fee-per-fill=" << std::setprecision(3)
            << execution.fixed_fee
            << " slippage-bps=" << execution.slippage_bps << '\n';
}
