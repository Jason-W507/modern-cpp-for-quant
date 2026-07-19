#include <cassert>
#include <iostream>

int main() {
  const int actual_fills = 1;
  const int expected_fills = 2;
  assert(actual_fills == expected_fills);

#ifdef NDEBUG
  std::cout << "assertion-check-disappeared\n";
#endif
}
