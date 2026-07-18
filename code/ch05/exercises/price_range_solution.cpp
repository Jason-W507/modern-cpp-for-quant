#include <iomanip>
#include <iostream>

#include "quant/ch05/market.hpp"

int main() {
  quant::ch05::MarketAnalyzer analyzer{"AAPL"};
  analyzer.add(quant::ch05::MarketQuote{"AAPL", 100.0, 10});
  analyzer.add(quant::ch05::MarketQuote{"AAPL", 101.0, 5});

  const auto result{analyzer.summary()};
  std::cout << std::fixed << std::setprecision(2)
            << "symbol=" << result.symbol
            << " range=" << result.high - result.low
            << " trend=" << quant::ch05::trend_name(result.trend) << '\n';
}
