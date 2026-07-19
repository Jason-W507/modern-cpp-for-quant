#include <iomanip>
#include <iostream>

double cash_after_buy(double cash, double price, int quantity, double fee) {
  const double notional = price * static_cast<double>(quantity);
  return cash - notional - fee;
}

int main() {
  const double observed = cash_after_buy(1'000.0, 100.0, 2, 1.0);
  if (observed != 799.0) {
    std::cerr << "cash update is still wrong\n";
    return 1;
  }
  std::cout << std::fixed << std::setprecision(0)
            << "fixed-cash=" << observed << '\n';
}
