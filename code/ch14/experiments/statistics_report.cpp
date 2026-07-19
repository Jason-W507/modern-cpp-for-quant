#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string_view>
#include <vector>

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

struct Summary final {
  double median_us;
  double iqr_us;
  long long p99_us;
  double throughput_ops_per_s;
};

Summary summarize(std::vector<long long> samples) {
  std::ranges::sort(samples);
  const std::size_t size = samples.size();
  const double median = median_of_sorted(samples, 0, size);
  const double q1 = median_of_sorted(samples, 0, size / 2);
  const double q3 = median_of_sorted(samples, (size + 1) / 2, size);
  const long long total_us =
      std::accumulate(samples.begin(), samples.end(), 0LL);
  return Summary{median,
                 q3 - q1,
                 samples[(99 * size + 99) / 100 - 1],
                 static_cast<double>(size) * 1'000'000.0 /
                     static_cast<double>(total_us)};
}

void print_summary(std::string_view name, const Summary& summary) {
  std::cout << name << " median-us=" << summary.median_us
            << " iqr-us=" << summary.iqr_us
            << " p99-us=" << summary.p99_us
            << " throughput-ops-per-s=" << summary.throughput_ops_per_s
            << '\n';
}

int main() {
  const std::vector<long long> stable{9, 10, 10, 10, 10, 10, 11, 12};
  const std::vector<long long> bursty{5, 5, 10, 10, 10, 10, 80, 100};

  std::cout << std::fixed << std::setprecision(2)
            << "stats-ok samples-per-run=8 serial-model=true\n";
  print_summary("stable", summarize(stable));
  print_summary("bursty", summarize(bursty));
}
