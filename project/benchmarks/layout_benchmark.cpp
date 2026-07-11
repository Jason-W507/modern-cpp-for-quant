#include <algorithm>
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

double median(std::vector<long long> samples) {
  std::ranges::sort(samples);
  const std::size_t middle = samples.size() / 2;
  return samples.size() % 2 == 0
             ? (static_cast<double>(samples[middle - 1]) +
                static_cast<double>(samples[middle])) /
                   2.0
             : static_cast<double>(samples[middle]);
}

long long interquartile_range(std::vector<long long> samples) {
  std::ranges::sort(samples);
  return samples[(samples.size() * 3) / 4] - samples[samples.size() / 4];
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

  const auto run_aos = [&] {
    double sum = 0.0;
    for (const Tick& tick : aos) {
      sum += tick.price * static_cast<double>(tick.quantity);
    }
    return sum;
  };
  const auto run_soa = [&] {
    double sum = 0.0;
    for (std::size_t i = 0; i < prices.size(); ++i) {
      sum += prices[i] * static_cast<double>(quantities[i]);
    }
    return sum;
  };

  // Warm both paths before collecting samples.
  const double warm_aos = run_aos();
  const double warm_soa = run_soa();
  constexpr int iterations = 20;
  std::vector<long long> aos_samples;
  std::vector<long long> soa_samples;
  aos_samples.reserve(iterations);
  soa_samples.reserve(iterations);
  double checksum_guard = warm_aos + warm_soa;
  bool checksums_match = warm_aos == warm_soa;

  for (int iteration = 0; iteration < iterations; ++iteration) {
    // Alternate order to reduce a fixed first/second measurement bias.
    if (iteration % 2 == 0) {
      const auto [aos_sum, aos_us] = measure(run_aos);
      const auto [soa_sum, soa_us] = measure(run_soa);
      aos_samples.push_back(aos_us);
      soa_samples.push_back(soa_us);
      checksums_match = checksums_match && aos_sum == soa_sum;
      checksum_guard += aos_sum + soa_sum;
    } else {
      const auto [soa_sum, soa_us] = measure(run_soa);
      const auto [aos_sum, aos_us] = measure(run_aos);
      aos_samples.push_back(aos_us);
      soa_samples.push_back(soa_us);
      checksums_match = checksums_match && aos_sum == soa_sum;
      checksum_guard += aos_sum + soa_sum;
    }
  }

  std::cout << "iterations=" << iterations
            << " aos_median_us=" << median(aos_samples)
            << " aos_iqr_us=" << interquartile_range(aos_samples)
            << " soa_median_us=" << median(soa_samples)
            << " soa_iqr_us=" << interquartile_range(soa_samples)
            << " checksum-match=" << std::boolalpha << checksums_match
            << " checksum=" << checksum_guard << '\n';
}
