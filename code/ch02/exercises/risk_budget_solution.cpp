#include <iostream>

int main() {
  double remaining_budget{3200.0};
  double risk_per_trade{750.0};
  int accepted_trades{};

  while (remaining_budget >= risk_per_trade) {
    remaining_budget -= risk_per_trade;
    ++accepted_trades;
  }

  std::cout << "accepted_trades=" << accepted_trades
            << " remaining_budget=" << remaining_budget << '\n';
}
