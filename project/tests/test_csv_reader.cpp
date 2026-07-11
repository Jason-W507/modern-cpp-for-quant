#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include "quant/csv.hpp"
#include "test_support.hpp"

int main() {
  std::istringstream valid{
      "timestamp_ms,symbol,price,quantity\n"
      "1000,AAPL,99.5,100\n"
      "2000,AAPL,101.0,80\n"};
  const auto parsed = quant::read_market_events(valid);
  test_support::require(parsed.has_value(), "valid CSV should parse");
  test_support::require(parsed.events.size() == 2,
                        "two rows should produce two events");
  test_support::require(parsed.events[0].symbol == "AAPL",
                        "symbol should be preserved");
  test_support::require(parsed.events[0].price == 99.5,
                        "price should be parsed");
  test_support::require(parsed.events[0].quantity == 100,
                        "quantity should be parsed");
  test_support::require(parsed.events[0].timestamp.time_since_epoch() ==
                            std::chrono::milliseconds{1000},
                        "timestamp should be milliseconds since epoch");

  std::istringstream invalid{
      "timestamp_ms,symbol,price,quantity\n"
      "1000,AAPL,99.5,not-a-number\n"};
  const auto rejected = quant::read_market_events(invalid);
  test_support::require(!rejected.has_value(),
                        "malformed CSV should be rejected");
  test_support::require(rejected.error.find("line 2") != std::string::npos,
                        "parse error should identify the source line");

  std::cout << "market data input seam ok\n";
}
