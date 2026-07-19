#include <iomanip>
#include <iostream>

#include "quant/ch08/execution.hpp"

int main() {
  quant::ch08::Portfolio portfolio{10'000.0};
  portfolio.apply(
      quant::ch08::Fill{"AAPL", quant::ch08::Side::buy, 25, 99.0});
  portfolio.apply(
      quant::ch08::Fill{"AAPL", quant::ch08::Side::sell, 10, 101.0});

  const auto result = portfolio.snapshot("AAPL", 101.0);
  std::cout << std::fixed << std::setprecision(2)
            << "quantity=" << result.quantity << " cash=" << result.cash
            << " equity=" << result.equity << '\n';
}
