#include <iostream>
#include <stdexcept>

#include "quant/ch10/portfolio.hpp"

int main() {
  quant::ch10::Portfolio portfolio{10'000.0};
  portfolio.apply_batch(
      {quant::ch10::Fill{"AAPL", quant::ch10::Side::buy, 10, 100.0}});
  const auto before = portfolio.snapshot("AAPL", 100.0);

  bool injected = false;
  try {
    portfolio.apply_batch(
        {quant::ch10::Fill{"AAPL", quant::ch10::Side::buy, 5, 100.0},
         quant::ch10::Fill{"AAPL", quant::ch10::Side::sell, 100, 100.0}});
  } catch (const std::logic_error&) {
    injected = true;
  }

  const auto after = portfolio.snapshot("AAPL", 100.0);
  const bool unchanged = after.quantity == before.quantity &&
                         after.cash == before.cash &&
                         after.equity == before.equity;
  if (!injected || !unchanged) {
    return 1;
  }
  std::cout << "injected=1 rolled_back=1 unchanged=1\n";
}
