#include <iostream>
#include <ranges>
#include <vector>

auto prices_above_100() {
  std::vector<double> prices{99.0, 101.0, 102.0};
  return prices | std::views::filter([](const double price) {
           return price > 100.0;
         });
}

int main() {
  auto dangling{prices_above_100()};
  double total{};
  for (const double price : dangling) {
    total += price;
  }
  std::cout << total << '\n';
}
