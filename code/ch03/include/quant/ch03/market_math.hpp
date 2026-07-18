#ifndef QUANT_CH03_MARKET_MATH_HPP
#define QUANT_CH03_MARKET_MATH_HPP

namespace quant::chapter3 {

double trade_notional(double price, int quantity);

double trade_notional(double price, int quantity, double multiplier);

void add_trade(double price, int quantity, double& total_notional,
               int& total_quantity);

double volume_weighted_price(double total_notional, int total_quantity);

}  // namespace quant::chapter3

#endif
