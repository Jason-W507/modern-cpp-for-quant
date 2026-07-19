#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

int main() {
  std::vector<double> values{1.0e16, -1.0e16, 1.0};
  const double forward{std::accumulate(values.begin(), values.end(), 0.0)};

  std::ranges::reverse(values);
  const double reverse{std::accumulate(values.begin(), values.end(), 0.0)};

  std::cout << "forward=" << forward << " reverse=" << reverse << '\n';
}
