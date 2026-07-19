#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <utility>
#include <vector>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

#ifndef CH14_BUILD_CONFIGURATION
#define CH14_BUILD_CONFIGURATION "direct"
#endif

struct Tick final {
  double price;
  std::int64_t quantity;
  std::int64_t timestamp;
};

void compiler_barrier() {
#if defined(_MSC_VER)
  _ReadWriteBarrier();
#elif defined(__GNUC__) || defined(__clang__)
  asm volatile("" ::: "memory");
#else
  std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

void consume_result(double value) {
#if defined(_MSC_VER)
  volatile double sink = value;
  (void)sink;
  _ReadWriteBarrier();
#elif defined(__GNUC__) || defined(__clang__)
  asm volatile("" : : "g"(value) : "memory");
#else
  volatile double sink = value;
  (void)sink;
  std::atomic_signal_fence(std::memory_order_seq_cst);
#endif
}

template <typename Work>
std::pair<double, long long> measure(Work&& work, std::size_t start_index) {
  const auto start = std::chrono::steady_clock::now();
  compiler_barrier();
  const double checksum = work(start_index);
  consume_result(checksum);
  compiler_barrier();
  const auto elapsed = std::chrono::steady_clock::now() - start;
  return {checksum,
          std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
              .count()};
}

double median_of_sorted(const std::vector<long long>& samples,
                        std::size_t begin, std::size_t end) {
  const std::size_t count = end - begin;
  const std::size_t middle = begin + count / 2;
  if (count % 2 == 1) {
    return static_cast<double>(samples[middle]);
  }
  return (static_cast<double>(samples[middle - 1]) +
          static_cast<double>(samples[middle])) /
         2.0;
}

std::pair<double, double> median_and_iqr(std::vector<long long> samples) {
  std::ranges::sort(samples);
  const std::size_t size = samples.size();
  const double median = median_of_sorted(samples, 0, size);
  const double q1 = median_of_sorted(samples, 0, size / 2);
  const double q3 = median_of_sorted(samples, (size + 1) / 2, size);
  return {median, q3 - q1};
}

void print_samples(std::string_view name,
                   const std::vector<long long>& samples) {
  const auto [median, iqr] = median_and_iqr(samples);
  std::cout << name << "-raw-us=";
  for (std::size_t index = 0; index < samples.size(); ++index) {
    if (index != 0) {
      std::cout << ',';
    }
    std::cout << samples[index];
  }
  std::cout << " median-us=" << median << " iqr-us=" << iqr << '\n';
}

constexpr std::string_view compiler_name() {
#if defined(__clang__)
  return "Clang " __clang_version__;
#elif defined(__GNUC__)
  return "GCC " __VERSION__;
#elif defined(_MSC_VER)
  return "MSVC";
#else
  return "unknown";
#endif
}

constexpr std::string_view architecture_name() {
#if defined(__x86_64__) || defined(_M_X64)
  return "x86-64";
#elif defined(__aarch64__) || defined(_M_ARM64)
  return "aarch64";
#else
  return "unknown";
#endif
}

int main() {
  constexpr std::size_t rows = 1'000'000;
  constexpr int warmups = 3;
  constexpr int samples = 21;
  constexpr double expected_checksum = 4'603'580'000.0;

  std::vector<Tick> aos;
  std::vector<double> prices;
  std::vector<std::int64_t> quantities;
  aos.reserve(rows);
  prices.reserve(rows);
  quantities.reserve(rows);
  for (std::size_t index = 0; index < rows; ++index) {
    const double price =
        90.0 + static_cast<double>(index % 200) * 0.01;
    const auto quantity =
        1 + static_cast<std::int64_t>(index % 100);
    aos.push_back(Tick{price, quantity, static_cast<std::int64_t>(index)});
    prices.push_back(price);
    quantities.push_back(quantity);
  }

  const auto run_aos = [&](std::size_t start_index) {
    double sum = 0.0;
    for (std::size_t index = start_index; index < aos.size(); ++index) {
      sum += aos[index].price * static_cast<double>(aos[index].quantity);
    }
    for (std::size_t index = 0; index < start_index; ++index) {
      sum += aos[index].price * static_cast<double>(aos[index].quantity);
    }
    return sum;
  };
  const auto run_soa = [&](std::size_t start_index) {
    double sum = 0.0;
    for (std::size_t index = start_index; index < prices.size(); ++index) {
      sum += prices[index] * static_cast<double>(quantities[index]);
    }
    for (std::size_t index = 0; index < start_index; ++index) {
      sum += prices[index] * static_cast<double>(quantities[index]);
    }
    return sum;
  };

  double checksum_guard = 0.0;
  bool checksums_match = true;
  for (int warmup = 0; warmup < warmups; ++warmup) {
    const std::size_t start_index =
        static_cast<std::size_t>(warmup + 1) * 104'729 % rows;
    const double aos_sum = run_aos(start_index);
    const double soa_sum = run_soa(start_index);
    checksum_guard += aos_sum + soa_sum;
    checksums_match = checksums_match && aos_sum == soa_sum &&
                      std::fabs(aos_sum - expected_checksum) < 0.01;
  }

  std::vector<long long> aos_samples;
  std::vector<long long> soa_samples;
  aos_samples.reserve(samples);
  soa_samples.reserve(samples);
  for (int sample = 0; sample < samples; ++sample) {
    const std::size_t start_index =
        static_cast<std::size_t>(sample + 1) * 7'919 % rows;
    if (sample % 2 == 0) {
      const auto [aos_sum, aos_us] = measure(run_aos, start_index);
      const auto [soa_sum, soa_us] = measure(run_soa, start_index);
      aos_samples.push_back(aos_us);
      soa_samples.push_back(soa_us);
      checksums_match = checksums_match && aos_sum == soa_sum &&
                        std::fabs(aos_sum - expected_checksum) < 0.01;
      checksum_guard += aos_sum + soa_sum;
    } else {
      const auto [soa_sum, soa_us] = measure(run_soa, start_index);
      const auto [aos_sum, aos_us] = measure(run_aos, start_index);
      soa_samples.push_back(soa_us);
      aos_samples.push_back(aos_us);
      checksums_match = checksums_match && aos_sum == soa_sum &&
                        std::fabs(aos_sum - expected_checksum) < 0.01;
      checksum_guard += aos_sum + soa_sum;
    }
  }

  std::cout << std::fixed << std::setprecision(2)
            << "benchmark-ok rows=" << rows << " samples=" << samples
            << " warmups=" << warmups
            << " alternating=true checksum-match=" << std::boolalpha
            << checksums_match << " rotating-start=true\n"
            << "environment compiler=" << compiler_name()
            << " architecture=" << architecture_name()
            << " configuration=" << CH14_BUILD_CONFIGURATION
#ifdef NDEBUG
            << " ndebug=true"
#else
            << " ndebug=false"
#endif
            << " clock=steady_clock\n";
  print_samples("aos", aos_samples);
  print_samples("soa", soa_samples);
  std::cout << "checksum-guard=" << checksum_guard
            << " conclusion=environment-specific\n";

  return checksums_match ? 0 : 2;
}
