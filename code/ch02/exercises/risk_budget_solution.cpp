#include <iomanip>
#include <iostream>

int main() {
  double remaining_budget{3200.0};
  const double risk_per_trade{750.0};
  int accepted_trades{};

  while (remaining_budget >= risk_per_trade) {
    remaining_budget -= risk_per_trade;
    ++accepted_trades;
  }

  std::cout << std::fixed << std::setprecision(2)
            << "accepted_trades=" << accepted_trades
            << " remaining_budget=" << remaining_budget << '\n';
}
