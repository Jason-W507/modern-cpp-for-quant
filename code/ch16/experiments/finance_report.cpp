#include "quant/ch16/numerics.hpp"

#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>

int main() {
  constexpr std::array returns{0.01, -0.02, 0.03};
  const std::int64_t rounded_cents =
      quant::ch16::round_major_to_cents(10.125);
  const double period_return = quant::ch16::simple_return(100.0, 102.5);
  const quant::ch16::Moments moments =
      quant::ch16::sample_moments(returns);
  const quant::ch16::TolerancePolicy tolerance_policy =
      quant::ch16::tolerance_from_budget(0.0, 32, 1.0);
  bool overflow_rejected = false;
  try {
    const double first_out_of_range_cent =
        std::ldexp(1.0, std::numeric_limits<std::int64_t>::digits) / 100.0;
    (void)quant::ch16::round_major_to_cents(first_out_of_range_cent);
  } catch (const std::overflow_error&) {
    overflow_rejected = true;
  }
  const bool valid = rounded_cents == 1013 &&
                     quant::ch16::almost_equal(
                         period_return, 0.025, tolerance_policy.absolute,
                         tolerance_policy.relative) &&
                     quant::ch16::almost_equal(
                         moments.mean, 0.006666666666666666,
                         tolerance_policy.absolute,
                         tolerance_policy.relative) &&
                     quant::ch16::almost_equal(
                         moments.sample_variance, 0.0006333333333333334,
                         tolerance_policy.absolute,
                         tolerance_policy.relative) &&
                     overflow_rejected;

  std::cout << "numeric-policy money=int64-cents rounded-cents="
            << rounded_cents << std::fixed << std::setprecision(6)
            << " return=" << period_return << " mean=" << moments.mean
            << " sample-variance=" << moments.sample_variance
            << " overflow-rejected=" << std::boolalpha << overflow_rejected
            << '\n';
  return valid ? 0 : 2;
}
