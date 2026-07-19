#ifndef QUANT_CH10_TEST_SUPPORT_HPP
#define QUANT_CH10_TEST_SUPPORT_HPP

#include <string>

namespace test_support {

void require(bool condition, const std::string& message);
bool close_to(double left, double right);

}  // namespace test_support

#endif
