#include "quant/ch16/numerics.hpp"

#include <array>
#include <iomanip>
#include <iostream>

int main() {
  const bool exact = 0.1 + 0.2 == 0.3;
  const bool tolerant =
      quant::ch16::almost_equal(0.1 + 0.2, 0.3, 1e-12, 1e-12);
  constexpr std::array values{1.0e16, 1.0, -1.0e16, 3.0};
  const double naive = quant::ch16::naive_sum(values);
  const double stable = quant::ch16::neumaier_sum(values);
  constexpr double oracle = 4.0;
  const bool valid = !exact && tolerant && naive == 3.0 && stable == oracle;

  std::cout << "stability-ok exact-0.1+0.2=" << std::boolalpha << exact
            << " tolerant=" << tolerant << std::fixed << std::setprecision(1)
            << " naive=" << naive << " stable=" << stable
            << " oracle=" << oracle << '\n';
  return valid ? 0 : 2;
}
