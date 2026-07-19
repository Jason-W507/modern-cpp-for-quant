#include <iomanip>
#include <iostream>

double cash_after_buy(double cash, double price, int quantity, double fee) {
  const double notional = price * static_cast<double>(quantity);
  return cash - notional + fee;
}

int main() {
  const double observed = cash_after_buy(1'000.0, 100.0, 2, 1.0);
  std::cerr << std::fixed << std::setprecision(0)
            << "observed-cash=" << observed << " expected-cash=799\n";
  return observed == 799.0 ? 0 : 2;
}
