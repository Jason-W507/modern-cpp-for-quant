#include <array>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <memory_resource>
#include <vector>

struct Quote final {
  double price;
  int quantity;
};

int main() {
  std::array<std::byte, 4'096> storage{};
  std::pmr::monotonic_buffer_resource arena{
      storage.data(), storage.size(), std::pmr::null_memory_resource()};
  std::pmr::vector<Quote> quotes{&arena};
  quotes.reserve(3);
  quotes.push_back(Quote{100.0, 2});
  quotes.push_back(Quote{101.0, 3});
  quotes.push_back(Quote{105.0, 1});

  double checksum = 0.0;
  for (const Quote& quote : quotes) {
    checksum += quote.price * static_cast<double>(quote.quantity);
  }
  if (checksum != 608.0) {
    std::cerr << "arena storage changed the calculation\n";
    return 1;
  }

  std::cout << std::fixed << std::setprecision(2)
            << "arena-ok rows=" << quotes.size() << " checksum=" << checksum
            << " buffer-bytes=" << storage.size() << '\n';
}
