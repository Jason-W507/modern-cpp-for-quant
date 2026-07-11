#include <cstdint>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

int main() {
  std::vector<std::int64_t> values(100'000);
  std::iota(values.begin(), values.end(), std::int64_t{1});

  std::int64_t left_sum = 0;
  std::int64_t right_sum = 0;
  const auto middle = values.begin() + static_cast<std::ptrdiff_t>(values.size() / 2);
  {
    std::jthread left([&] {
      left_sum = std::accumulate(values.begin(), middle, std::int64_t{0});
    });
    std::jthread right([&] {
      right_sum = std::accumulate(middle, values.end(), std::int64_t{0});
    });
  }  // jthread destructors join before the two results are read

  std::cout << "sum=" << left_sum + right_sum << '\n';
}
