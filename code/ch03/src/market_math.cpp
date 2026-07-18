#include "quant/ch03/market_math.hpp"

namespace quant::chapter3 {

double trade_notional(double price, int quantity) {
  return price * quantity;
}

double trade_notional(double price, int quantity, double multiplier) {
  return trade_notional(price, quantity) * multiplier;
}

void add_trade(double price, int quantity, double& total_notional,
               int& total_quantity) {
  total_notional += trade_notional(price, quantity);
  total_quantity += quantity;
}

double volume_weighted_price(double total_notional, int total_quantity) {
  if (total_quantity == 0) {
    return 0.0;
  }
  return total_notional / total_quantity;
}

}  // namespace quant::chapter3
