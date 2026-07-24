#pragma once

#include "quant/capstone/factor_kernel.hpp"

#include <cmath>
#include <limits>
#include <span>
#include <stdexcept>

namespace quant::capstone::detail {

inline std::size_t checked_element_count(std::size_t rows,
                                         std::size_t columns) {
  if (columns != 0 &&
      rows > std::numeric_limits<std::size_t>::max() / columns) {
    throw std::invalid_argument{"factor batch shape overflows size_t"};
  }
  return rows * columns;
}

inline void validate_factor_inputs(FactorBatchView batch,
                                   std::span<const double> weights) {
  const std::size_t element_count =
      checked_element_count(batch.rows, batch.columns);
  if (batch.columns == 0 || weights.size() != batch.columns ||
      batch.values.size() != element_count) {
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
