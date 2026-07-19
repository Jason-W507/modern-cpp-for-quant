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
  const bool valid =
      quant::ch16::almost_equal(moments.mean, 0.025, 1e-15, 1e-12) &&
      quant::ch16::almost_equal(
          moments.sample_variance, 0.10125, 1e-15, 1e-12);

  std::cout << "return-stats-ok periods=" << moments.count << std::fixed
            << std::setprecision(6) << " mean=" << moments.mean
            << " sample-variance=" << moments.sample_variance << '\n';
  return valid ? 0 : 2;
}
