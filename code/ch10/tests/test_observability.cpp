#include <iostream>
#include <sstream>
#include <string>

#include "quant/ch10/pipeline.hpp"
#include "test_support.hpp"

int main() {
  std::istringstream input{
      "symbol,side,quantity,price\n"
      "AAPL,buy,10,100.0\n"
      "AAPL,buy,oops,100.0\n"
      "AAPL,buy,15,99.0\n"};
  std::ostringstream log;
  quant::ch10::Portfolio portfolio{10'000.0};

  const auto metrics = quant::ch10::process_fill_csv(input, log, portfolio);
  const auto snapshot = portfolio.snapshot("AAPL", 99.0);

  test_support::require(
      log.str() ==
          "error line=3 field=quantity message=expected a positive integer\n",
      "batch boundary should record the contextual parse error exactly once");
  test_support::require(
      metrics.rows_seen == 3 && metrics.rows_accepted == 2 &&
          metrics.rows_rejected == 1 && metrics.batches_committed == 2 &&
          metrics.batches_rolled_back == 0,
      "metrics should describe accepted, rejected, and committed work");
  test_support::require(
      snapshot.quantity == 25 && test_support::close_to(snapshot.cash, 7'515.0),
      "processing should recover and commit later valid rows");

  std::istringstream extreme_input{
      "symbol,side,quantity,price\n"
      "AAPL,buy,9223372036854775807,1e-300\n"
      "AAPL,buy,1,1e-300\n"
      "AAPL,sell,1,1e-300\n"};
  std::ostringstream extreme_log;
  quant::ch10::Portfolio extreme_portfolio{10'000.0};
  const auto extreme_metrics = quant::ch10::process_fill_csv(
      extreme_input, extreme_log, extreme_portfolio);
  test_support::require(
      extreme_log.str() ==
          "error line=3 field=portfolio message="
          "buy quantity exceeds portfolio range\n",
      "overflow should retain row context and be recorded exactly once");
  test_support::require(
      extreme_metrics.rows_accepted == 2 &&
          extreme_metrics.rows_rejected == 1 &&
          extreme_metrics.batches_committed == 2 &&
          extreme_metrics.batches_rolled_back == 1,
      "overflow recovery should roll back one row and continue processing");

  std::cout << "observability boundary ok\n";
}
