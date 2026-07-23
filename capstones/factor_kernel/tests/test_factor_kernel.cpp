#include "quant/capstone/factor_kernel.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

int main() {
  const double missing = std::numeric_limits<double>::quiet_NaN();
  const std::vector<double> values{
      1.0, 2.0, missing,
      2.0, missing, 4.0,
      missing, missing, missing,
      3.0, 1.0, -1.0,
  };
  const std::vector<double> weights{0.5, -0.25, 0.25};
  const quant::capstone::FactorBatchView batch{values, 4, 3};
  const auto scores = quant::capstone::weighted_factor(batch, weights);

  const bool values_match =
      scores.size() == 4 && std::fabs(scores[0]) < 1e-12 &&
      std::fabs(scores[1] - 8.0 / 3.0) < 1e-12 &&
      std::isnan(scores[2]) && std::fabs(scores[3] - 1.0) < 1e-12;

  bool rejected_shape = false;
  try {
    static_cast<void>(quant::capstone::weighted_factor(
        quant::capstone::FactorBatchView{values, 3, 3}, weights));
  } catch (const std::invalid_argument&) {
    rejected_shape = true;
  }

  if (!values_match || !rejected_shape) {
    std::cerr << "factor kernel oracle mismatch\n";
    return 2;
  }
  std::cout << "factor-kernel-tests-ok rows=4 finite=3 missing=1\n";
}
