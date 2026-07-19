#include <array>
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

struct Quote final {
  std::int64_t price_cents;
  std::int64_t quantity;
};

std::int64_t scan_range(const std::vector<Quote>& quotes, std::size_t first,
                        std::size_t last) {
  std::int64_t notional_cents = 0;
  for (std::size_t index = first; index < last; ++index) {
    notional_cents += quotes[index].price_cents * quotes[index].quantity;
  }
  return notional_cents;
}

std::int64_t scan_sequential(const std::vector<Quote>& quotes) {
  return scan_range(quotes, 0, quotes.size());
}

std::int64_t scan_with_threads(const std::vector<Quote>& quotes) {
  const std::size_t middle = quotes.size() / 2;
  std::array<std::int64_t, 2> partials{};
  {
    std::jthread first{[&] { partials[0] = scan_range(quotes, 0, middle); }};
    std::jthread second{
        [&] { partials[1] = scan_range(quotes, middle, quotes.size()); }};
  }
  return partials[0] + partials[1];
}

std::int64_t scan_with_tasks(const std::vector<Quote>& quotes) {
  const std::size_t middle = quotes.size() / 2;
  auto first = std::async(std::launch::async, [&] {
    return scan_range(quotes, 0, middle);
  });
  auto second = std::async(std::launch::async, [&] {
    return scan_range(quotes, middle, quotes.size());
  });
  return first.get() + second.get();
}

int main() {
  const std::vector<Quote> quotes{{10'000, 2}, {10'100, 3}, {9'900, 1},
                                  {10'200, 4}, {10'300, 2}, {9'800, 5},
                                  {10'400, 1}, {10'050, 2}};
  constexpr std::int64_t expected_cents = 201'100;

  const std::int64_t sequential = scan_sequential(quotes);
  const std::int64_t threaded = scan_with_threads(quotes);
  const std::int64_t tasked = scan_with_tasks(quotes);
  const bool matches = sequential == expected_cents &&
                       threaded == expected_cents && tasked == expected_cents;

  std::cout << "pipeline-ok rows=" << quotes.size()
            << " sequential-cents=" << sequential
            << " thread-cents=" << threaded << " task-cents=" << tasked
            << " checksum-match=" << std::boolalpha << matches << '\n';
  return matches ? 0 : 2;
}
