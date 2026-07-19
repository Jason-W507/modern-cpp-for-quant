#ifndef QUANT_CH08_TEST_SUPPORT_HPP
#define QUANT_CH08_TEST_SUPPORT_HPP

#include <string_view>

namespace test_support {

void require(bool condition, std::string_view message);
bool close_to(double left, double right);

}  // namespace test_support

#endif
