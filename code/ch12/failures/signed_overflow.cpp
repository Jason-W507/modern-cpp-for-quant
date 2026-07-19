#include <limits>

int main() {
  volatile int quantity = std::numeric_limits<int>::max();
  quantity += 1;
  return quantity;
}
