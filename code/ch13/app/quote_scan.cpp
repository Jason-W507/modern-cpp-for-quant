#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  const std::vector<double> prices{99.0, 100.0, 101.5, 98.0, 102.0};
  const std::vector<std::int64_t> quantities{2, 3, 4, 5, 1};
  constexpr double threshold = 100.0;

  std::size_t selected = 0;
  double notional = 0.0;
  for (std::size_t index = 0; index < prices.size(); ++index) {
    if (prices[index] >= threshold) {
      ++selected;
      notional += prices[index] * static_cast<double>(quantities[index]);
    }
  }

  std::cout << std::fixed << std::setprecision(2)
            << "quote-scan-ok rows=" << prices.size()
            << " selected=" << selected << " notional=" << notional << '\n';
}
