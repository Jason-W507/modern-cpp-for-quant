#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <vector>

#include "quant/ch07/trade.hpp"

int main() {
  const std::vector<quant::ch07::Trade> trades{
      {"AAPL", quant::ch07::Side::buy, 10, 100.0},
      {"MSFT", quant::ch07::Side::buy, 4, 200.0},
      {"AAPL", quant::ch07::Side::sell, 5, 102.0},
      {"AAPL", quant::ch07::Side::buy, 10, 101.0},
  };

  auto apple = trades |
               std::views::filter([](const quant::ch07::Trade& trade) {
                 return trade.symbol == "AAPL";
               });
  std::vector<quant::ch07::Trade> ranked;
  std::ranges::copy(apple, std::back_inserter(ranked));
  std::ranges::sort(
      ranked, [](const quant::ch07::Trade& left,
                 const quant::ch07::Trade& right) {
        return left.price > right.price;
      });

  std::cout << std::fixed << std::setprecision(2) << "top=";
  bool first{true};
  for (const auto& trade : ranked | std::views::take(2)) {
    if (!first) {
      std::cout << ',';
    }
    std::cout << trade.symbol << '@' << trade.price;
    first = false;
  }
  std::cout << '\n';
}
