#include "quant/capstone/factor_kernel.hpp"
#include "quant/capstone/detail/factor_validation.hpp"

#include <cmath>
#include <limits>

namespace quant::capstone {

std::vector<double> weighted_factor(FactorBatchView batch,
                                    std::span<const double> weights) {
  detail::validate_factor_inputs(batch, weights);

  std::vector<double> result;
  result.reserve(batch.rows);
  for (std::size_t row = 0; row < batch.rows; ++row) {
    double weighted_sum{};
    double observed_weight{};
    for (std::size_t column = 0; column < batch.columns; ++column) {
      const double value = batch.values[row * batch.columns + column];
      if (std::isnan(value)) {
        continue;
      }
      weighted_sum += value * weights[column];
      observed_weight += std::fabs(weights[column]);
    }
    result.push_back(observed_weight == 0.0
                         ? std::numeric_limits<double>::quiet_NaN()
                         : weighted_sum / observed_weight);
  }
  return result;
}

}  // namespace quant::capstone
