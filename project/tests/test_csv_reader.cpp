#include <chrono>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <utility>

#include "quant/csv.hpp"
#include "test_support.hpp"

namespace {

class FailingInputBuffer final : public std::streambuf {
 public:
  explicit FailingInputBuffer(std::string data) : data_(std::move(data)) {}

 protected:
  int_type underflow() override {
    if (offset_ == data_.size()) {
      throw std::ios_base::failure{"injected read failure"};
    }
    current_ = data_[offset_++];
    setg(&current_, &current_, &current_ + 1);
    return traits_type::to_int_type(current_);
  }

 private:
  std::string data_;
  std::size_t offset_{};
  char current_{};
};

}  // namespace

int main() {
  std::istringstream valid{
      "timestamp_ms,symbol,price,quantity\n"
      "1000,AAPL,99.5,100\n"
      "2000,AAPL,101.0,80\n"};
  const auto parsed = quant::read_market_events(valid);
  test_support::require(parsed.has_value(), "valid CSV should parse");
  test_support::require(parsed.events.size() == 2,
                        "two rows should produce two events");
  test_support::require(parsed.events[0].symbol() == "AAPL",
                        "symbol should be preserved");
  test_support::require(parsed.events[0].price() == 99.5,
                        "price should be parsed");
  test_support::require(parsed.events[0].quantity() == 100,
                        "quantity should be parsed");
  test_support::require(parsed.events[0].timestamp().time_since_epoch() ==
                            std::chrono::milliseconds{1000},
                        "timestamp should be milliseconds since epoch");

  std::istringstream crlf{
      "timestamp_ms,symbol,price,quantity\r\n"
      "1000,AAPL,99.5,100\r\n"};
  const auto parsed_crlf = quant::read_market_events(crlf);
  test_support::require(parsed_crlf.has_value() && parsed_crlf.events.size() == 1,
                        "CRLF CSV should parse at the public input seam");

  std::istringstream invalid{
      "timestamp_ms,symbol,price,quantity\n"
      "1000,AAPL,99.5,not-a-number\n"};
  const auto rejected = quant::read_market_events(invalid);
  test_support::require(!rejected.has_value(),
                        "malformed CSV should be rejected");
  test_support::require(rejected.error.has_value() &&
                            rejected.error->line == 2 &&
                            rejected.error->field == quant::MarketEventField::quantity &&
                            rejected.error->code == quant::ParseErrorCode::invalid_number,
                        "parse error should identify line, field, and reason");

  std::istringstream non_finite{
      "timestamp_ms,symbol,price,quantity\n"
      "1000,AAPL,nan,10\n"};
  const auto rejected_nan = quant::read_market_events(non_finite);
  test_support::require(
      rejected_nan.error.has_value() && rejected_nan.error->line == 2 &&
          rejected_nan.error->field == quant::MarketEventField::price &&
          rejected_nan.error->code == quant::ParseErrorCode::non_finite,
      "non-finite prices should retain a structured diagnostic");

  test_support::require(
      quant::format_parse_error(*rejected.error) ==
          "line 2: quantity invalid-number",
      "CLI rendering should be separate from structured parsing");

  FailingInputBuffer failing_buffer{
      "timestamp_ms,symbol,price,quantity\n1000,AAPL,99.5,100\n"};
  std::istream failing_stream{&failing_buffer};
  const auto read_failure = quant::read_market_events(failing_stream);
  test_support::require(
      read_failure.error.has_value() && read_failure.error->line == 3 &&
          read_failure.error->field == quant::MarketEventField::stream &&
          read_failure.error->code == quant::ParseErrorCode::read_failure,
      "stream failures should not be mistaken for ordinary EOF");

  std::cout << "market data input seam ok\n";
}
