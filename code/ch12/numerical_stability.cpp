#include <cmath>
#include <iostream>
#include <vector>

double compensated_sum(const std::vector<double>& values) {
  double sum = 0.0;
  double correction = 0.0;
  for (const double value : values) {
    const double next = sum + value;
    if (std::abs(sum) >= std::abs(value)) {
      correction += (sum - next) + value;
    } else {
      correction += (value - next) + sum;
    }
    sum = next;
  }
  return sum + correction;
}

int main() {
  const std::vector<double> values{1e16, 1.0, -1e16};
  double naive = 0.0;
  for (const double value : values) {
    naive += value;
  }
  std::cout << "naive=" << naive
            << " compensated=" << compensated_sum(values) << '\n';
}
