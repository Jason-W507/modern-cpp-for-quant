#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "quant/ch07/csv_reader.hpp"
#include "quant/ch07/pipeline.hpp"

int main(const int argc, const char* argv[]) {
  if (argc != 3) {
    std::cerr << "error=usage: ch07_ranges_pipeline <trades.csv> <symbol>\n";
    return 2;
  }

  std::ifstream input{argv[1]};
  if (!input) {
    std::cerr << "error=cannot open input file\n";
    return 2;
  }

  try {
    const auto trades{quant::ch07::read_trades(input)};
    const auto result{quant::ch07::summarize_trades(trades, argv[2])};
    std::cout << std::fixed << std::setprecision(2)
              << "symbol=" << result.symbol << " trades=" << result.trades
              << " first_sell=" << result.first_sell
              << " low=" << result.low << " high=" << result.high
              << " gross=" << result.gross
              << " net_quantity=" << result.net_quantity << '\n';
  } catch (const std::exception& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 2;
  }
}
