#include <iostream>

double basis_points(const double& rate) {
  return rate * 10000.0;
}

int main() {
  const double rate{0.0015};
  const double converted{basis_points(rate)};

  std::cout << "rate=" << rate << " basis_points=" << converted << '\n';
  return 0;
}
