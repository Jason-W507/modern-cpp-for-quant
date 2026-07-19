#include "test_support.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace test_support {

void require(bool condition, std::string_view message) {
  if (!condition) {
    std::cerr << "failure: " << message << '\n';
    std::exit(1);
  }
}

bool close_to(double left, double right) {
  return std::abs(left - right) < 1e-9;
}

}  // namespace test_support
