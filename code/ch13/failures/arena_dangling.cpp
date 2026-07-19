#include <memory_resource>
#include <vector>

double* escaped_arena_price() {
  std::pmr::monotonic_buffer_resource arena;
  std::pmr::vector<double> prices{&arena};
  prices.push_back(100.0);
  return prices.data();
}

int main() {
  double* price = escaped_arena_price();
  return *price > 0.0 ? 0 : 1;
}
