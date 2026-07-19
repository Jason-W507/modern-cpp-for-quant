#include <memory>

int main() {
  auto prices = std::make_unique<double[]>(2);
  prices[0] = 100.0;
  prices[1] = 101.0;
  prices[2] = 102.0;
  return prices[0] > 0.0 ? 0 : 1;
}
