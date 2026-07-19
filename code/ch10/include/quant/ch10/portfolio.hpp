#ifndef QUANT_CH10_PORTFOLIO_HPP
#define QUANT_CH10_PORTFOLIO_HPP

#include <string>
#include <vector>

#include "quant/ch10/domain.hpp"

namespace quant::ch10 {

class Portfolio final {
 public:
  explicit Portfolio(double initial_cash);

  void apply_batch(const std::vector<Fill>& fills);
  PortfolioSnapshot snapshot(const std::string& symbol,
                             double mark_price) const;

 private:
  void apply_one(const Fill& fill);
  void swap(Portfolio& other) noexcept;

  double cash_;
  std::string symbol_;
  std::int64_t quantity_{};
};

}  // namespace quant::ch10

#endif
