#include "quant/ch12/portfolio.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>

namespace quant::ch12 {

Portfolio::Portfolio(double initial_cash) : cash_{initial_cash} {
  if (!std::isfinite(initial_cash) || initial_cash < 0.0) {
    throw std::invalid_argument{"initial cash must be finite and nonnegative"};
  }
}

void Portfolio::buy(double price, std::int64_t quantity, double fee) {
  if (!std::isfinite(price) || price <= 0.0 || quantity <= 0 ||
      !std::isfinite(fee) || fee < 0.0) {
    throw std::invalid_argument{"buy inputs must be finite and valid"};
  }
  if (quantity > std::numeric_limits<std::int64_t>::max() - quantity_) {
    throw std::overflow_error{"position quantity would overflow"};
  }
  const double cost = price * static_cast<double>(quantity) + fee;
  if (!std::isfinite(cost) || cost > cash_) {
    throw std::logic_error{"buy cost exceeds available cash"};
  }
  quantity_ += quantity;
  cash_ -= cost;
}

PortfolioSnapshot Portfolio::snapshot(double mark_price) const {
  if (!std::isfinite(mark_price) || mark_price <= 0.0) {
    throw std::invalid_argument{"mark price must be finite and positive"};
  }
  return PortfolioSnapshot{
      quantity_, cash_, cash_ + mark_price * static_cast<double>(quantity_)};
}

}  // namespace quant::ch12
