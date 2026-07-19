#include "quant/ch10/portfolio.hpp"

#include <cassert>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace quant::ch10 {

Portfolio::Portfolio(double initial_cash) : cash_(initial_cash) {
  if (!std::isfinite(cash_) || cash_ < 0.0) {
    throw std::invalid_argument{"initial cash must be finite and non-negative"};
  }
}

void Portfolio::apply_batch(const std::vector<Fill>& fills) {
  Portfolio candidate{*this};
  for (const Fill& fill : fills) {
    candidate.apply_one(fill);
  }
  swap(candidate);
}

PortfolioSnapshot Portfolio::snapshot(const std::string& symbol,
                                      double mark_price) const {
  detail::require_symbol(symbol);
  detail::require_price(mark_price);
  if (!symbol_.empty() && symbol_ != symbol) {
    throw std::logic_error{"snapshot symbol does not match portfolio"};
  }
  assert(quantity_ >= 0);
  return PortfolioSnapshot{
      symbol, quantity_, cash_,
      cash_ + static_cast<double>(quantity_) * mark_price};
}

void Portfolio::apply_one(const Fill& fill) {
  const double notional = static_cast<double>(fill.quantity()) * fill.price();
  if (!std::isfinite(notional)) {
    throw std::overflow_error{"fill notional must be finite"};
  }
  if (!symbol_.empty() && symbol_ != fill.symbol()) {
    throw std::logic_error{"portfolio supports one symbol per run"};
  }
  if (fill.side() == Side::sell && quantity_ < fill.quantity()) {
    throw std::logic_error{"sell quantity exceeds position"};
  }
  if (fill.side() == Side::buy && cash_ < notional) {
    throw std::logic_error{"buy notional exceeds cash"};
  }
  if (fill.side() == Side::buy &&
      quantity_ > std::numeric_limits<std::int64_t>::max() - fill.quantity()) {
    throw std::overflow_error{"buy quantity exceeds portfolio range"};
  }

  std::int64_t next_quantity{};
  double next_cash{};
  if (fill.side() == Side::buy) {
    next_quantity = quantity_ + fill.quantity();
    next_cash = cash_ - notional;
  } else {
    next_quantity = quantity_ - fill.quantity();
    next_cash = cash_ + notional;
  }
  if (!std::isfinite(next_cash)) {
    throw std::overflow_error{"portfolio cash must remain finite"};
  }

  if (symbol_.empty()) {
    symbol_ = fill.symbol();
  }
  quantity_ = next_quantity;
  cash_ = next_cash;
  assert(quantity_ >= 0);
  assert(std::isfinite(cash_) && cash_ >= 0.0);
}

void Portfolio::swap(Portfolio& other) noexcept {
  using std::swap;
  swap(cash_, other.cash_);
  symbol_.swap(other.symbol_);
  swap(quantity_, other.quantity_);
}

}  // namespace quant::ch10
