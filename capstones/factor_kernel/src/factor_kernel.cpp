#include "quant/capstone/factor_kernel.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace quant::capstone {

std::vector<double> weighted_factor(FactorBatchView batch,
                                    std::span<const double> weights) {
  if (batch.columns == 0 || weights.size() != batch.columns ||
      batch.values.size() != batch.rows * batch.columns) {
    throw std::invalid_argument{"factor batch shape mismatch"};
  }
  for (const double weight : weights) {
    if (!std::isfinite(weight)) {
      throw std::invalid_argument{"factor weights must be finite"};
    }
  }

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
      if (!std::isfinite(value)) {
        throw std::invalid_argument{"factor values must be finite or NaN"};
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
