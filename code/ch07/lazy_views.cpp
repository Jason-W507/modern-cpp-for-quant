#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

int main() {
  std::vector<double> prices{99.0, 101.0, 80.0};
  auto selected = prices | std::views::filter([](const double price) {
                    return price >= 100.0;
                  }) |
                  std::views::transform([](const double price) {
                    return price * 2.0;
                  });

  prices[0] = 110.0;

  std::cout << std::fixed << std::setprecision(2) << "selected=";
  bool first{true};
  for (const double value : selected) {
    if (!first) {
      std::cout << ',';
    }
    std::cout << value;
    first = false;
  }
  std::cout << '\n';
}
