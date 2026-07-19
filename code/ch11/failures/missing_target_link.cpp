#include <cstdint>

std::int64_t load_market_quantity();

int main() {
  return load_market_quantity() == 25 ? 0 : 1;
}
