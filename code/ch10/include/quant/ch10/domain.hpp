#ifndef QUANT_CH10_DOMAIN_HPP
#define QUANT_CH10_DOMAIN_HPP

#include <cstdint>
#include <string>
#include <utility>

namespace quant::ch10 {

enum class Side : std::uint8_t { buy, sell };

namespace detail {

void require_symbol(const std::string& symbol);
void require_price(double price);
void require_quantity(std::int64_t quantity);

}  // namespace detail

class Fill final {
 public:
  Fill(std::string symbol, Side side, std::int64_t quantity, double price)
      : symbol_(std::move(symbol)),
        side_(side),
        quantity_(quantity),
        price_(price) {
    detail::require_symbol(symbol_);
    detail::require_quantity(quantity_);
    detail::require_price(price_);
  }

  const std::string& symbol() const { return symbol_; }
  Side side() const { return side_; }
  std::int64_t quantity() const { return quantity_; }
  double price() const { return price_; }

 private:
  std::string symbol_;
  Side side_;
  std::int64_t quantity_;
  double price_;
};

struct PortfolioSnapshot final {
  std::string symbol;
  std::int64_t quantity{};
  double cash{};
  double equity{};
};

}  // namespace quant::ch10

#endif
