#include <iostream>
#include <limits>
#include <stdexcept>

#include "quant/ch08/execution.hpp"
#include "test_support.hpp"

bool invalid_order_is_rejected() {
  try {
    const quant::ch08::Order invalid{"AAPL", quant::ch08::Side::buy, 0};
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

bool invalid_event_is_rejected() {
  try {
    const quant::ch08::MarketEvent invalid{
        "AAPL", std::numeric_limits<double>::quiet_NaN(), 10};
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

bool invalid_fill_is_rejected() {
  try {
    const quant::ch08::Fill invalid{
        "AAPL", quant::ch08::Side::buy, 10,
        std::numeric_limits<double>::infinity()};
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

int main() {
  test_support::require(invalid_order_is_rejected(),
      "zero quantity should be rejected");
  test_support::require(invalid_event_is_rejected(),
      "non-finite event price should be rejected");
  test_support::require(invalid_fill_is_rejected(),
      "non-finite fill price should be rejected");

  quant::ch08::Portfolio portfolio{100.0};
  const auto before = portfolio.snapshot("AAPL", 10.0);
  bool rejected = false;
  try {
    portfolio.apply(
        quant::ch08::Fill{"AAPL", quant::ch08::Side::buy, 11, 10.0});
  } catch (const std::logic_error&) {
    rejected = true;
  }
  const auto after = portfolio.snapshot("AAPL", 10.0);
  test_support::require(rejected, "insufficient cash should reject a fill");
  test_support::require(after.quantity == before.quantity &&
                            test_support::close_to(after.cash, before.cash),
                        "rejected fill should preserve portfolio state");

  std::cout << "domain invariants ok\n";
}
