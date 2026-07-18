#include "../include/quant/ch05/market.hpp"

int main() {
  quant::ch05::MarketQuote quote{"AAPL", 100.0, 10};
  return quote.quantity_;
}
