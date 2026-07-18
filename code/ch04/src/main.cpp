#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "quant/ch04/csv_stats.hpp"

int main(const int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "error=usage: ch04_csv_stats <market.csv>\n";
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

  const int quantity{quant::ch04::total_quantity(quantities)};
  double notional{0.0};
  if (!quant::ch04::total_notional(prices, quantities, notional)) {
    std::cerr << "error=internal column length mismatch\n";
    return 2;
  }
  double vwap{0.0};
  if (quantity != 0) {
    vwap = notional / quantity;
  }

  std::cout << std::fixed << std::setprecision(2)
            << "rows=" << symbols.size() << " total_quantity=" << quantity
            << " total_notional=" << notional << " vwap=" << vwap << '\n';
}
