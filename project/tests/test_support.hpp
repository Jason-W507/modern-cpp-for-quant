#pragma once

#include <cmath>
#include <stdexcept>

namespace test_support {

inline void require(bool condition, const char* message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

inline bool close_to(double lhs, double rhs, double tolerance = 1e-9) {
  return std::abs(lhs - rhs) <= tolerance;
}

}  // namespace test_support
