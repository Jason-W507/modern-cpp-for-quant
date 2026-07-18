#include <array>
#include <iostream>
#include <string>
#include <vector>

int main() {
  const std::string symbol{"AAPL"};
  std::vector<double> prices{100.0, 101.5, 99.0};
  prices.reserve(8);
  prices.push_back(100.0);

  const std::array<std::string, 3> header{"symbol", "price", "quantity"};
  double total{0.0};
  for (const double price : prices) {
    total += price;
  }

  std::cout << "symbol=" << symbol << " symbol_size=" << symbol.size()
            << " prices_size=" << prices.size()
            << " capacity_at_least_8=" << (prices.capacity() >= 8)
            << " header_last=" << header[2] << " total=" << total << '\n';
}
