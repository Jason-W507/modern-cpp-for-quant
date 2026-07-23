#include "quant/capstone/eigen_factor.hpp"
#include "quant/capstone/factor_kernel.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <span>
#include <string_view>
#include <vector>

namespace {

using Samples = std::vector<std::int64_t>;

template <typename Function>
std::int64_t measure_once(Function&& function, double& checksum) {
  const auto start = std::chrono::steady_clock::now();
  const auto result = function();
  const auto stop = std::chrono::steady_clock::now();
  checksum = std::accumulate(result.begin(), result.end(), 0.0,
                             [](double sum, double value) {
                               return std::isnan(value) ? sum : sum + value;
                             });
  return std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start)
      .count();
}

std::int64_t median(Samples samples) {
  std::sort(samples.begin(), samples.end());
  return samples[samples.size() / 2];
}

double median(std::vector<double> samples) {
  std::sort(samples.begin(), samples.end());
  return samples[samples.size() / 2];
}

void write_samples(std::ostream& output, std::span<const std::int64_t> samples) {
  output << '[';
  for (std::size_t index = 0; index < samples.size(); ++index) {
    output << (index == 0 ? "" : ", ") << samples[index];
  }
  output << ']';
}

}  // namespace

int main(int argc, char** argv) {
  constexpr std::size_t rows = 32'768;
  constexpr std::size_t columns = 32;
  constexpr std::size_t repeats = 31;
  std::vector<double> values(rows * columns);
  std::vector<double> weights(columns);
  for (std::size_t index = 0; index < values.size(); ++index) {
    values[index] = index % 97 == 0
                        ? std::numeric_limits<double>::quiet_NaN()
                        : std::sin(static_cast<double>(index) * 0.001);
  }
  for (std::size_t column = 0; column < columns; ++column) {
    weights[column] = -1.0 + 2.0 * static_cast<double>(column) /
                                 static_cast<double>(columns - 1);
  }
  const quant::capstone::FactorBatchView batch{values, rows, columns};
  double scalar_checksum{};
  double eigen_checksum{};
  const auto scalar_warmup = quant::capstone::weighted_factor(batch, weights);
  const auto eigen_warmup =
      quant::capstone::weighted_factor_eigen(batch, weights);
  if (scalar_warmup.size() != rows || eigen_warmup.size() != rows) {
    std::cerr << "factor comparison warmup failed\n";
    return 1;
  }
  Samples scalar;
  Samples eigen;
  std::vector<double> paired_ratios;
  scalar.reserve(repeats);
  eigen.reserve(repeats);
  paired_ratios.reserve(repeats);
  const auto scalar_run = [&] {
    return measure_once(
        [&] { return quant::capstone::weighted_factor(batch, weights); },
        scalar_checksum);
  };
  const auto eigen_run = [&] {
    return measure_once(
        [&] { return quant::capstone::weighted_factor_eigen(batch, weights); },
        eigen_checksum);
  };
  for (std::size_t repeat = 0; repeat < repeats; ++repeat) {
    std::int64_t scalar_sample{};
    std::int64_t eigen_sample{};
    if (repeat % 2 == 0) {
      scalar_sample = scalar_run();
      eigen_sample = eigen_run();
    } else {
      eigen_sample = eigen_run();
      scalar_sample = scalar_run();
    }
    scalar.push_back(scalar_sample);
    eigen.push_back(eigen_sample);
    paired_ratios.push_back(static_cast<double>(eigen_sample) /
                            static_cast<double>(scalar_sample));
  }
  if (std::abs(scalar_checksum - eigen_checksum) > 1e-9) {
    std::cerr << "factor comparison checksum mismatch\n";
    return 2;
  }
  const auto scalar_median = median(scalar);
  const auto eigen_median = median(eigen);
  const auto paired_ratio_median = median(paired_ratios);
  if (argc == 3 && std::string_view{argv[1]} == "--json") {
    std::ofstream output{argv[2]};
    if (!output) {
      std::cerr << "cannot open factor comparison report\n";
      return 3;
    }
    output << "{\n  \"schema\": 1,\n  \"workload\": {\"rows\": " << rows
           << ", \"columns\": " << columns << ", \"repeats\": "
           << repeats << "},\n  \"scalar_samples_ns\": ";
    write_samples(output, scalar);
    output << ",\n  \"eigen_samples_ns\": ";
    write_samples(output, eigen);
    output << ",\n  \"paired_eigen_over_scalar\": [";
    for (std::size_t index = 0; index < paired_ratios.size(); ++index) {
      output << (index == 0 ? "" : ", ") << std::setprecision(6)
             << paired_ratios[index];
    }
    output << ']';
    output << ",\n  \"paired_ratio_median\": " << std::setprecision(6)
           << paired_ratio_median;
    output << ",\n  \"scalar_median_ns\": " << scalar_median
           << ",\n  \"eigen_median_ns\": " << eigen_median
           << ",\n  \"eigen_over_scalar\": " << std::setprecision(6)
           << static_cast<double>(eigen_median) /
                  static_cast<double>(scalar_median)
           << ",\n  \"checksum\": " << std::setprecision(17) << scalar_checksum
           << ",\n  \"interpretation\": \"Eigen preserves the oracle; adopt it only if the paired median improves\",\n"
              "  \"limitations\": [\"single host and process\", \"wall-clock samples, not hardware counters\", \"both paths allocate their result; Eigen also materializes value and mask matrices\"]\n}\n";
  }
  std::cout << "factor-compare-ok rows=" << rows
            << " scalar_median_ns=" << scalar_median
            << " eigen_median_ns=" << eigen_median << '\n';
}
