#include <exception>
#include <iostream>

#include "quant/ch05/market.hpp"

int main() {
  try {
    const quant::ch05::MarketQuote valid{"AAPL", 100.0, 10};
    std::cout << "valid=" << valid.symbol() << '@' << valid.price();
    const quant::ch05::MarketQuote invalid{"AAPL", -1.0, 10};
    std::cout << " unexpected=" << invalid.price();
  } catch (const std::exception& error) {
    std::cout << " rejected=" << error.what();
  }
  quant::ch05::MarketAnalyzer analyzer{"AAPL"};
  try {
    analyzer.add(quant::ch05::MarketQuote{"MSFT", 50.0, 10});
  } catch (const std::exception& error) {
    std::cout << " mismatch=" << error.what();
  }
  std::cout << '\n';
}
