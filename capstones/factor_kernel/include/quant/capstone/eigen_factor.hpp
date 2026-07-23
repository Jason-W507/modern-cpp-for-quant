#pragma once

#include "quant/capstone/factor_kernel.hpp"

#include <span>
#include <vector>

namespace quant::capstone {

std::vector<double> weighted_factor_eigen(
    FactorBatchView batch, std::span<const double> weights);

}  // namespace quant::capstone
