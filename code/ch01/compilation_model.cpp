#include <iostream>
#include <iomanip>
#include <vector>

struct Trade final {
  double price;
  int quantity;
};

[[nodiscard]] double notional(const std::vector<Trade>& trades) {
  double total = 0.0;
  for (const Trade& trade : trades) {
    total += trade.price * static_cast<double>(trade.quantity);
  }
  return total;
}

int main() {
  const std::vector<Trade> trades{{100.0, 500}, {100.5, 250}, {99.5, 503}};
  std::cout << std::fixed << std::setprecision(2)
            << "notional=" << notional(trades) << '\n';
}
