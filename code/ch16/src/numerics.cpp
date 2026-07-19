#include "quant/ch16/numerics.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace quant::ch16 {

double naive_sum(const std::span<const double> values) {
  double total = 0.0;
  for (const double value : values) {
    total += value;
  }
  return total;
}

double neumaier_sum(const std::span<const double> values) {
  double total = 0.0;
  double correction = 0.0;
  for (const double value : values) {
    const double next = total + value;
    if (std::abs(total) >= std::abs(value)) {
      correction += (total - next) + value;
    } else {
      correction += (value - next) + total;
    }
    total = next;
  }
  return total + correction;
}

bool almost_equal(const double lhs, const double rhs,
                  const double absolute_tolerance,
                  const double relative_tolerance) {
  if (absolute_tolerance < 0.0 || relative_tolerance < 0.0) {
    throw std::invalid_argument{"tolerances must be nonnegative"};
  }
  const double difference = std::abs(lhs - rhs);
  const double scale = std::max(std::abs(lhs), std::abs(rhs));
  return difference <=
         std::max(absolute_tolerance, relative_tolerance * scale);
}

Moments sample_moments(const std::span<const double> values) {
  if (values.size() < 2) {
    throw std::invalid_argument{"sample variance needs at least two values"};
  }
  Moments result{};
  double squared_deviation_sum = 0.0;
  for (const double value : values) {
    ++result.count;
    const double delta = value - result.mean;
    result.mean += delta / static_cast<double>(result.count);
    const double next_delta = value - result.mean;
    squared_deviation_sum += delta * next_delta;
  }
  result.sample_variance =
      squared_deviation_sum / static_cast<double>(result.count - 1);
  return result;
}

std::int64_t round_major_to_cents(const double major_units) {
  if (!std::isfinite(major_units)) {
    throw std::invalid_argument{"money amount must be finite"};
  }
  const double scaled = major_units * 100.0;
  const double limit =
      static_cast<double>(std::numeric_limits<std::int64_t>::max());
  if (std::abs(scaled) > limit) {
    throw std::overflow_error{"money amount exceeds int64 cents"};
  }
  return static_cast<std::int64_t>(std::round(scaled));
}

double simple_return(const double previous_price,
                     const double current_price) {
  if (!(previous_price > 0.0) || !std::isfinite(previous_price) ||
      !std::isfinite(current_price)) {
    throw std::invalid_argument{"prices must be finite and previous positive"};
  }
  return current_price / previous_price - 1.0;
}

BatchSum sum_batch(const BatchView view) {
  if (view.dtype != DType::float64) {
    return BoundaryError::wrong_dtype;
  }
  if (view.stride_bytes != static_cast<std::ptrdiff_t>(sizeof(double))) {
    return BoundaryError::non_contiguous;
  }
  if (view.data == nullptr && view.size != 0) {
    return BoundaryError::null_data;
  }
  const auto* data = static_cast<const double*>(view.data);
  return neumaier_sum({data, view.size});
}

std::variant<ReturnedView, BoundaryError> validate_returned_view(
    ReturnedView view) {
  if (view.data == nullptr && view.size != 0) {
    return BoundaryError::null_data;
  }
  if (view.size != 0 && !view.owner) {
    return BoundaryError::owner_missing;
  }
  return view;
}

std::string_view dtype_name(const DType dtype) {
  switch (dtype) {
    case DType::float64:
      return "float64";
    case DType::float32:
      return "float32";
    case DType::int64:
      return "int64";
  }
  return "unknown";
}

std::string_view boundary_error_name(const BoundaryError error) {
  switch (error) {
    case BoundaryError::wrong_dtype:
      return "wrong-dtype";
    case BoundaryError::non_contiguous:
      return "non-contiguous";
    case BoundaryError::null_data:
      return "null-data";
    case BoundaryError::owner_missing:
      return "owner-missing";
  }
  return "unknown";
}

}  // namespace quant::ch16
