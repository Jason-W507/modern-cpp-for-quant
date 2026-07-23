#include "quant/capstone/eigen_factor.hpp"
#include "quant/capstone/detail/factor_validation.hpp"

#include <Eigen/Dense>

#include <cmath>
#include <limits>

namespace quant::capstone {

std::vector<double> weighted_factor_eigen(
    FactorBatchView batch, std::span<const double> weights) {
  detail::validate_factor_inputs(batch, weights);
  using Matrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic,
                               Eigen::RowMajor>;
  Matrix values(batch.rows, batch.columns);
  Matrix observed(batch.rows, batch.columns);
  for (std::size_t row = 0; row < batch.rows; ++row) {
    for (std::size_t column = 0; column < batch.columns; ++column) {
      const double value = batch.values[row * batch.columns + column];
      const bool present = !std::isnan(value);
      values(row, column) = present ? value : 0.0;
      observed(row, column) = present ? 1.0 : 0.0;
    }
  }
  const Eigen::Map<const Eigen::VectorXd> weight_vector(weights.data(),
                                                         weights.size());
  const Eigen::VectorXd numerator = values * weight_vector;
  const Eigen::VectorXd denominator = observed * weight_vector.cwiseAbs();
  std::vector<double> result(batch.rows);
  for (std::size_t row = 0; row < batch.rows; ++row) {
    result[row] = denominator[row] == 0.0
                      ? std::numeric_limits<double>::quiet_NaN()
                      : numerator[row] / denominator[row];
  }
  return result;
}

}  // namespace quant::capstone
