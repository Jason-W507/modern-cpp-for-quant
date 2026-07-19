#include <iostream>
#include <span>
#include <vector>

std::span<const double> make_dangling_view() {
  const std::vector<double> temporary(64, 1.25);
  return {temporary.data(), temporary.size()};
}

int main() {
  const std::span<const double> view = make_dangling_view();
  const volatile double observed = view.front();  // Intentional use-after-free.
  std::cout << observed << '\n';
}
