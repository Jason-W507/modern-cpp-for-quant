#include "test_support.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

namespace test_support {

void require(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "failure: " << message << '\n';
    std::exit(1);
  }
}

bool close_to(double left, double right) {
  return std::fabs(left - right) < 1e-9;
}

}  // namespace test_support
