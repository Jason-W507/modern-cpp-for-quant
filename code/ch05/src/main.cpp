#include <fstream>
#include <iomanip>
#include <iostream>
#include <exception>

#include "quant/ch05/csv_reader.hpp"
#include "quant/ch05/market.hpp"

int main(const int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "error=usage: ch05_market_analyzer <market.csv> <symbol>\n";
    return 2;
  }

  try {
    std::ifstream input{argv[1]};
    if (!input) {
      std::cerr << "error=cannot open input file\n";
      return 2;
    }
    const auto quotes{quant::ch05::read_market_csv(input)};
    quant::ch05::MarketAnalyzer analyzer{argv[2]};
    for (const auto& quote : quotes) {
      if (quote.symbol() == argv[2]) {
        analyzer.add(quote);
      }
    }
    const auto result{analyzer.summary()};
    std::cout << std::fixed << std::setprecision(2)
              << "symbol=" << result.symbol << " rows=" << result.rows
              << " first=" << result.first_price
              << " last=" << result.last_price
              << " return=" << result.return_percent << '%'
              << " low=" << result.low << " high=" << result.high
              << " trend=" << quant::ch05::trend_name(result.trend)
              << " quantity=" << result.total_quantity << '\n';
  } catch (const std::exception& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 2;
  }
}
