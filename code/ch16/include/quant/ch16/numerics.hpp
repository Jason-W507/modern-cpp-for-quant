#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <variant>

namespace quant::ch16 {

struct Moments {
  std::size_t count{};
  double mean{};
  double sample_variance{};
};

struct TolerancePolicy {
  double input_absolute_error{};
  std::size_t rounded_operations{};
  double scale{};
  double absolute{};
  double relative{};
};

[[nodiscard]] double naive_sum(std::span<const double> values);
[[nodiscard]] double neumaier_sum(std::span<const double> values);
[[nodiscard]] bool almost_equal(double lhs, double rhs,
                                double absolute_tolerance,
                                double relative_tolerance);
[[nodiscard]] TolerancePolicy tolerance_from_budget(
    double input_absolute_error, std::size_t rounded_operations,
    double scale);
[[nodiscard]] Moments sample_moments(std::span<const double> values);
[[nodiscard]] std::int64_t round_major_to_cents(double major_units);
[[nodiscard]] double simple_return(double previous_price,
                                   double current_price);

enum class DType { float64, float32, int64, unsupported };
enum class BoundaryError { wrong_dtype, non_contiguous, null_data, owner_missing };

struct BatchView {
  const void* data{};
  std::size_t size{};
  DType dtype{DType::float64};
  std::ptrdiff_t stride_bytes{static_cast<std::ptrdiff_t>(sizeof(double))};
};

struct ReturnedView {
  const double* data{};
  std::size_t size{};
  std::shared_ptr<const void> owner{};
};

using BatchSum = std::variant<double, BoundaryError>;

[[nodiscard]] std::optional<BoundaryError> validate_batch(BatchView view);
[[nodiscard]] BatchSum sum_batch(BatchView view);
[[nodiscard]] std::variant<ReturnedView, BoundaryError> validate_returned_view(
    ReturnedView view);
[[nodiscard]] std::string_view dtype_name(DType dtype);
[[nodiscard]] std::string_view boundary_error_name(BoundaryError error);

}  // namespace quant::ch16
