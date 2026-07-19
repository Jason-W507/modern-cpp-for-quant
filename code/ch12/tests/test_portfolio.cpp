#include <cmath>
#include <iomanip>
#include <iostream>

#include "quant/ch12/portfolio.hpp"

int main() {
  quant::ch12::Portfolio portfolio{1'000.0};
  portfolio.buy(100.0, 2, 1.0);
  const auto snapshot = portfolio.snapshot(110.0);

  const bool correct = snapshot.quantity == 2 &&
                       std::abs(snapshot.cash - 799.0) < 1e-12 &&
                       std::abs(snapshot.equity - 1'019.0) < 1e-12;
  if (!correct) {
    std::cerr << "portfolio fee behavior mismatch\n";
    return 1;
  }

  std::cout << std::fixed << std::setprecision(2)
            << "portfolio-fee-ok cash=" << snapshot.cash
            << " equity=" << snapshot.equity << '\n';
}
