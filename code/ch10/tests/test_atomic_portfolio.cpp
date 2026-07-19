#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "quant/ch10/portfolio.hpp"
#include "test_support.hpp"

int main() {
  quant::ch10::Portfolio portfolio{10'000.0};
  portfolio.apply_batch(
      {quant::ch10::Fill{"AAPL", quant::ch10::Side::buy, 10, 100.0}});
  const auto before = portfolio.snapshot("AAPL", 101.0);

  bool rejected = false;
  try {
    portfolio.apply_batch(
        {quant::ch10::Fill{"AAPL", quant::ch10::Side::buy, 5, 100.0},
         quant::ch10::Fill{"MSFT", quant::ch10::Side::buy, 1, 420.0}});
  } catch (const std::logic_error&) {
    rejected = true;
  }

  const auto after = portfolio.snapshot("AAPL", 101.0);
  test_support::require(rejected, "mixed-symbol batch should be rejected");
  test_support::require(
      after.quantity == before.quantity &&
          test_support::close_to(after.cash, before.cash) &&
          test_support::close_to(after.equity, before.equity),
      "rejected batch must preserve the observable portfolio snapshot");

  quant::ch10::Portfolio extreme{10'000.0};
  extreme.apply_batch({quant::ch10::Fill{
      "AAPL", quant::ch10::Side::buy,
      std::numeric_limits<std::int64_t>::max(), 1e-300}});
  const auto before_overflow = extreme.snapshot("AAPL", 1e-300);
  bool overflow_rejected = false;
  try {
    extreme.apply_batch(
        {quant::ch10::Fill{"AAPL", quant::ch10::Side::buy, 1, 1e-300}});
  } catch (const std::overflow_error&) {
    overflow_rejected = true;
  }
  const auto after_overflow = extreme.snapshot("AAPL", 1e-300);
  test_support::require(overflow_rejected,
                        "quantity overflow should be rejected explicitly");
  test_support::require(
      after_overflow.quantity == before_overflow.quantity &&
          test_support::close_to(after_overflow.cash, before_overflow.cash),
      "overflow rejection must preserve the portfolio snapshot");

  std::cout << "strong exception guarantee ok\n";
}
