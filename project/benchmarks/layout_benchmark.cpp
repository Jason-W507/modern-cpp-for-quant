#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

struct Tick final {
  double price;
  std::int64_t quantity;
  std::int64_t timestamp;
};

template <typename Work>
auto measure(Work&& work) {
  const auto start = std::chrono::steady_clock::now();
  const double checksum = work();
  const auto elapsed = std::chrono::steady_clock::now() - start;
  return std::pair{checksum,
                   std::chrono::duration_cast<std::chrono::microseconds>(
                       elapsed)
                       .count()};
}

int main() {
  constexpr std::size_t size = 1'000'000;
  std::vector<Tick> aos;
  std::vector<double> prices;
  std::vector<std::int64_t> quantities;
  aos.reserve(size);
  prices.reserve(size);
  quantities.reserve(size);
  for (std::size_t i = 0; i < size; ++i) {
    const double price = 90.0 + static_cast<double>(i % 200) * 0.01;
    const std::int64_t quantity = 1 + static_cast<std::int64_t>(i % 100);
    aos.push_back(Tick{price, quantity, static_cast<std::int64_t>(i)});
    prices.push_back(price);
    quantities.push_back(quantity);
  }

  const auto [aos_sum, aos_us] = measure([&] {
    double sum = 0.0;
    for (const Tick& tick : aos) {
      sum += tick.price * static_cast<double>(tick.quantity);
    }
    return sum;
  });
  const auto [soa_sum, soa_us] = measure([&] {
    double sum = 0.0;
    for (std::size_t i = 0; i < prices.size(); ++i) {
      sum += prices[i] * static_cast<double>(quantities[i]);
    }
    return sum;
  });

  std::cout << "aos_us=" << aos_us << " soa_us=" << soa_us
            << " checksum-match=" << std::boolalpha << (aos_sum == soa_sum)
            << '\n';
}
