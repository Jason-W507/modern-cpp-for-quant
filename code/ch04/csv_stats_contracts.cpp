#include <iostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

#include "quant/ch04/csv_stats.hpp"

namespace {

class FailingBuffer final : public std::streambuf {
 public:
  FailingBuffer(std::string data, const std::size_t fail_after)
      : data_{std::move(data)}, fail_after_{fail_after} {}

 protected:
  int_type underflow() override {
    if (position_ >= fail_after_) {
      throw std::runtime_error{"simulated input failure"};
    }
    if (position_ >= data_.size()) {
      return traits_type::eof();
    }
    return traits_type::to_int_type(data_[position_]);
  }

  int_type uflow() override {
    const int_type character{underflow()};
    if (!traits_type::eq_int_type(character, traits_type::eof())) {
      ++position_;
    }
    return character;
  }

 private:
  std::string data_;
  std::size_t fail_after_{};
  std::size_t position_{};
};

}  // namespace

int main() {
  std::vector<std::string> symbols;
  std::vector<double> prices;
  std::vector<int> quantities;
  std::string error;

  std::istringstream crlf{
      "symbol,price,quantity\r\nAAPL,100,10\r\nAAPL,101,5\r\n"};
  const bool crlf_ok{quant::ch04::read_market_csv(
      crlf, symbols, prices, quantities, error)};

  const std::string partial{
      "symbol,price,quantity\nAAPL,100,10\nMSFT,80,5\n"};
  const std::size_t fail_after{partial.find("MSFT")};
  FailingBuffer buffer{partial, fail_after};
  std::istream failing_input{&buffer};
  const bool read_failed{!quant::ch04::read_market_csv(
      failing_input, symbols, prices, quantities, error)};
  const bool failure_cleared{symbols.empty() && prices.empty() &&
                             quantities.empty() &&
                             error == "line 3: input read failure"};

  double notional{123.0};
  const bool mismatch_rejected{!quant::ch04::total_notional(
      std::vector<double>{100.0, 101.0}, std::vector<int>{10}, notional)};

  std::cout << "crlf_ok=" << crlf_ok << " read_failure="
            << (read_failed && failure_cleared)
            << " mismatch_rejected=" << (mismatch_rejected && notional == 0.0)
            << '\n';
}
