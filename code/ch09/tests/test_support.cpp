#include "test_support.hpp"

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

}  // namespace test_support
