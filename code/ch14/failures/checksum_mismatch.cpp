#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

int main() {
  const std::vector<double> prices{99.0, 100.0, 101.5, 98.0, 102.0};
  const std::vector<std::int64_t> quantities{2, 3, 4, 5, 1};
  constexpr double threshold = 100.0;

  double baseline = 0.0;
  double candidate = 0.0;
  for (std::size_t index = 0; index < prices.size(); ++index) {
    if (prices[index] >= threshold) {
      baseline += prices[index] * static_cast<double>(quantities[index]);
    }
    if (prices[index] > threshold) {  // Deliberate semantic drift.
      candidate += prices[index] * static_cast<double>(quantities[index]);
    }
  }

  if (baseline != candidate) {
    std::cerr << "benchmark-invalid checksum-mismatch baseline=" << baseline
              << " candidate=" << candidate << '\n';
    return 2;
  }
  return 0;
}
