#include "quant/ch16/numerics.hpp"

#include <array>
#include <iomanip>
#include <iostream>
#include <variant>
#include <vector>

int main() {
  constexpr std::array values{0.25, -0.125, 0.5, 0.375};

  double scalar_total = 0.0;
  std::size_t scalar_calls = 0;
  for (const double value : values) {
    scalar_total += quant::ch16::neumaier_sum({&value, 1});
    ++scalar_calls;
  }

  const std::vector<double> copied{values.begin(), values.end()};
  const double copied_total = quant::ch16::neumaier_sum(copied);
  const quant::ch16::BatchView borrowed{
      values.data(), values.size(), quant::ch16::DType::float64,
      static_cast<std::ptrdiff_t>(sizeof(double))};
  const auto borrowed_result = quant::ch16::sum_batch(borrowed);
  const double borrowed_total = std::get<double>(borrowed_result);

  const quant::ch16::BatchView wrong_dtype{
      values.data(), values.size(), quant::ch16::DType::float32,
      static_cast<std::ptrdiff_t>(sizeof(double))};
  const quant::ch16::BatchView wrong_layout{
      values.data(), values.size(), quant::ch16::DType::float64, 16};
  const quant::ch16::ReturnedView ownerless{values.data(), values.size(), {}};

  const bool dtype_rejected =
      std::get<quant::ch16::BoundaryError>(quant::ch16::sum_batch(wrong_dtype)) ==
      quant::ch16::BoundaryError::wrong_dtype;
  const bool layout_rejected =
      std::get<quant::ch16::BoundaryError>(quant::ch16::sum_batch(wrong_layout)) ==
      quant::ch16::BoundaryError::non_contiguous;
  const auto view_result = quant::ch16::validate_returned_view(ownerless);
  const bool owner_rejected =
      std::get<quant::ch16::BoundaryError>(view_result) ==
      quant::ch16::BoundaryError::owner_missing;
  const bool valid = scalar_calls == values.size() && scalar_total == 1.0 &&
                     copied_total == 1.0 && borrowed_total == 1.0 &&
                     dtype_rejected && layout_rejected && owner_rejected;

  std::cout << "boundary-ok rows=" << values.size()
            << " scalar-calls=" << scalar_calls
            << " bulk-copies=1 zero-copy-borrows=1" << std::fixed
            << std::setprecision(6) << " checksum=" << borrowed_total << '\n'
            << "dtype-error expected=float64 actual="
            << quant::ch16::dtype_name(wrong_dtype.dtype) << '\n'
            << "layout-error expected=c-contiguous stride-bytes="
            << wrong_layout.stride_bytes << '\n'
            << "ownership-error temporary-view-rejected owner-missing="
            << std::boolalpha << owner_rejected << '\n'
            << "gil-policy release=cpp-loop reacquire=python-api\n";
  return valid ? 0 : 2;
}
