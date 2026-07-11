#include <iostream>
#include <ranges>
#include <vector>

#include "quant/types.hpp"

int main() {
  const std::vector<quant::Fill> fills{
      {{}, "AAPL", quant::Side::buy, 10, 187.5},
      {{}, "AAPL", quant::Side::sell, 4, 188.0},
      {{}, "MSFT", quant::Side::buy, 5, 420.0},
      {{}, "MSFT", quant::Side::sell, 2, 421.0},
  };

  auto buys = fills | std::views::filter([](const quant::Fill& fill) {
                return fill.side == quant::Side::buy;
              });

  double positive_notional = 0.0;
  for (const quant::Fill& fill : buys) {
    positive_notional += fill.price * static_cast<double>(fill.quantity);
  }
  std::cout << "positive-notional=" << positive_notional << '\n';
}
