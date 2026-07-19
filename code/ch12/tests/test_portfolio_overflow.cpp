#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "quant/ch12/portfolio.hpp"

int main() {
  quant::ch12::Portfolio portfolio{std::numeric_limits<double>::max()};
  portfolio.buy(1.0, std::numeric_limits<std::int64_t>::max(), 0.0);

  try {
    portfolio.buy(1.0, 1, 0.0);
  } catch (const std::overflow_error&) {
    return 0;
  }

  std::cerr << "position quantity overflow was not rejected\n";
  return 1;
}
