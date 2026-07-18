#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "quant/ch04/csv_stats.hpp"

int main(const int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "error=usage: ch04_symbol_summary_solution <market.csv> <symbol>\n";
    return 2;
  }

  std::ifstream input{argv[1]};
  if (!input) {
    std::cerr << "error=cannot open input file\n";
    return 2;
  }

  std::vector<std::string> symbols;
  std::vector<double> prices;
  std::vector<int> quantities;
  std::string error;
  if (!quant::ch04::read_market_csv(input, symbols, prices, quantities,
                                    error)) {
    std::cerr << "error=" << error << '\n';
    return 2;
  }

  const std::string wanted{argv[2]};
  std::size_t rows{0};
  int quantity{0};
  double notional{0.0};
  for (std::size_t index{0}; index < symbols.size(); ++index) {
    if (symbols[index] == wanted) {
      ++rows;
      quantity += quantities[index];
      notional += prices[index] * quantities[index];
    }
  }

  std::cout << std::fixed << std::setprecision(2) << "symbol=" << wanted
            << " rows=" << rows << " quantity=" << quantity
            << " notional=" << notional << '\n';
}
