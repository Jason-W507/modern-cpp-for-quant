#include "quant/capstone/eigen_factor.hpp"
#include "quant/capstone/factor_kernel.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

namespace {

template <typename Function>
bool rejects_invalid_argument(Function&& function) {
  try {
    function();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

}  // namespace

int main() {
  const std::vector<double> values{1.0, 2.0, NAN, 2.0, NAN, 4.0,
                                   NAN, NAN, NAN, 3.0, 1.0, -1.0};
  const std::vector<double> weights{0.5, -0.25, 0.25};
  const quant::capstone::FactorBatchView batch{values, 4, 3};
  const auto scalar = quant::capstone::weighted_factor(batch, weights);
  const auto eigen = quant::capstone::weighted_factor_eigen(batch, weights);
  for (std::size_t index = 0; index < scalar.size(); ++index) {
    if (std::isnan(scalar[index]) != std::isnan(eigen[index]) ||
        (!std::isnan(scalar[index]) &&
         std::abs(scalar[index] - eigen[index]) > 1e-12)) {
      std::cerr << "Eigen factor mismatch\n";
      return 2;
    }
  }
  const std::vector<double> nan_weight{
      0.5, std::numeric_limits<double>::quiet_NaN(), 0.25};
  if (!rejects_invalid_argument(
          [&] { quant::capstone::weighted_factor_eigen(batch, nan_weight); })) {
    std::cerr << "Eigen factor accepted a NaN weight\n";
    return 3;
  }
  const quant::capstone::FactorBatchView zero_column_batch{{}, 0, 0};
  if (!rejects_invalid_argument([&] {
        quant::capstone::weighted_factor_eigen(zero_column_batch, {});
      })) {
    std::cerr << "Eigen factor accepted a zero-column batch\n";
    return 4;
  }
  std::cout << "eigen-factor-ok rows=4 scalar-match=1\n";
}
