#ifndef QUANT_CH12_PORTFOLIO_HPP
#define QUANT_CH12_PORTFOLIO_HPP

#include <cstdint>

namespace quant::ch12 {

struct PortfolioSnapshot {
  std::int64_t quantity;
  double cash;
  double equity;
};

class Portfolio {
 public:
  explicit Portfolio(double initial_cash);

  void buy(double price, std::int64_t quantity, double fee);
  [[nodiscard]] PortfolioSnapshot snapshot(double mark_price) const;

 private:
  std::int64_t quantity_{};
  double cash_{};
};

}  // namespace quant::ch12

#endif
