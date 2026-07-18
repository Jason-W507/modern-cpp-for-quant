#include "quant/ch03/market_math.hpp"

#include <iomanip>
#include <iostream>

int main() {
  double total_notional{};
  int total_quantity{};

  double price{};
  int quantity{};
  while (std::cin >> price >> quantity) {
    quant::chapter3::add_trade(price, quantity, total_notional,
                               total_quantity);
  }

  if (!std::cin.eof()) {
    std::cerr << "input-error: expected price and quantity\n";
    return 2;
  }

  const double vwap{
      quant::chapter3::volume_weighted_price(total_notional, total_quantity)};
  std::cout << std::fixed << std::setprecision(2)
            << "notional=" << total_notional << " quantity=" << total_quantity
            << " vwap=" << vwap << '\n';
  return 0;
}
