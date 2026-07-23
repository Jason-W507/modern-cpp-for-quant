#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace quant::capstone {

struct FactorBatchView final {
  std::span<const double> values;
  std::size_t rows{};
  std::size_t columns{};
};

[[nodiscard]] std::vector<double> weighted_factor(
    FactorBatchView batch, std::span<const double> weights);

}  // namespace quant::capstone
