#include "quant/ch16/numerics.hpp"

#include <array>
#include <iomanip>
#include <iostream>

int main() {
  constexpr std::array prices{100.0, 125.0, 100.0};
  std::array<double, prices.size() - 1> returns{};
  for (std::size_t index = 1; index < prices.size(); ++index) {
    returns[index - 1] =
        quant::ch16::simple_return(prices[index - 1], prices[index]);
  }
  const quant::ch16::Moments moments =
      quant::ch16::sample_moments(returns);
  const quant::ch16::TolerancePolicy tolerance_policy =
      quant::ch16::tolerance_from_budget(0.0, 24, 1.0);
  const bool valid =
      quant::ch16::almost_equal(moments.mean, 0.025,
                               tolerance_policy.absolute,
                               tolerance_policy.relative) &&
      quant::ch16::almost_equal(
          moments.sample_variance, 0.10125, tolerance_policy.absolute,
          tolerance_policy.relative);

  std::cout << "return-stats-ok periods=" << moments.count << std::fixed
            << std::setprecision(6) << " mean=" << moments.mean
            << " sample-variance=" << moments.sample_variance << '\n';
  return valid ? 0 : 2;
}
