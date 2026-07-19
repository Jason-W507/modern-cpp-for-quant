#include "quant/ch16/numerics.hpp"

#include <array>
#include <iomanip>
#include <iostream>

int main() {
  constexpr std::array returns{0.01, -0.02, 0.03};
  const std::int64_t rounded_cents =
      quant::ch16::round_major_to_cents(10.125);
  const double period_return = quant::ch16::simple_return(100.0, 102.5);
  const quant::ch16::Moments moments =
      quant::ch16::sample_moments(returns);
  const bool valid = rounded_cents == 1013 &&
                     quant::ch16::almost_equal(
                         period_return, 0.025, 1e-15, 1e-12) &&
                     quant::ch16::almost_equal(
                         moments.mean, 0.006666666666666666, 1e-15, 1e-12) &&
                     quant::ch16::almost_equal(
                         moments.sample_variance, 0.0006333333333333334,
                         1e-15, 1e-12);

  std::cout << "numeric-policy money=int64-cents rounded-cents="
            << rounded_cents << std::fixed << std::setprecision(6)
            << " return=" << period_return << " mean=" << moments.mean
            << " sample-variance=" << moments.sample_variance << '\n';
  return valid ? 0 : 2;
}
