#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

#if defined(_MSC_VER)
#define CH14_NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define CH14_NOINLINE __attribute__((noinline))
#else
#define CH14_NOINLINE
#endif

CH14_NOINLINE std::vector<double> decode_stage(
    const std::vector<std::int64_t>& raw_prices) {
  std::vector<double> prices;
  prices.reserve(raw_prices.size());
  for (const std::int64_t raw : raw_prices) {
    prices.push_back(static_cast<double>(raw) / 10'000.0);
  }
  return prices;
}

CH14_NOINLINE double score_stage(const std::vector<double>& prices) {
  double score = 0.0;
  for (int repeat = 0; repeat < 300; ++repeat) {
    for (const double price : prices) {
      score += std::sqrt(price + static_cast<double>(repeat) * 0.001);
    }
  }
  return score;
}

CH14_NOINLINE double aggregate_stage(const std::vector<double>& prices) {
  double sum = 0.0;
  for (std::size_t index = 0; index < prices.size(); index += 10) {
    sum += prices[index];
  }
  return sum;
}

int main() {
  constexpr std::size_t rows = 200'000;
  std::vector<std::int64_t> raw_prices;
  raw_prices.reserve(rows);
  for (std::size_t index = 0; index < rows; ++index) {
    raw_prices.push_back(
        900'000 + static_cast<std::int64_t>(index % 20'000));
  }

  const std::vector<double> prices = decode_stage(raw_prices);
  const double checksum = score_stage(prices) + aggregate_stage(prices);
  const bool checksum_finite = std::isfinite(checksum) && checksum > 0.0;

  std::cout << std::fixed << std::setprecision(2)
            << "profile-workload-ok rows=" << rows
            << " checksum-finite=" << std::boolalpha << checksum_finite
            << " checksum=" << checksum << '\n';
  return checksum_finite ? 0 : 2;
}
