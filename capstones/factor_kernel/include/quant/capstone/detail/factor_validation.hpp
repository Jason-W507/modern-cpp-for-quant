#pragma once

#include "quant/capstone/factor_kernel.hpp"

#include <cmath>
#include <span>
#include <stdexcept>

namespace quant::capstone::detail {

inline void validate_factor_inputs(FactorBatchView batch,
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
  for (const double value : batch.values) {
    if (!std::isfinite(value) && !std::isnan(value)) {
      throw std::invalid_argument{"factor values must be finite or NaN"};
    }
  }
}

}  // namespace quant::capstone::detail
