#ifndef QUANT_CH09_DOMAIN_HPP
#define QUANT_CH09_DOMAIN_HPP

#include <cstdint>
#include <string>
#include <utility>

namespace quant::ch09 {

enum class Side : std::uint8_t { buy, sell };

namespace detail {

void require_symbol(const std::string& symbol);
void require_price(double price);
void require_quantity(std::int64_t quantity);

}  // namespace detail

class MarketEvent final {
 public:
  MarketEvent(std::string symbol, double price, std::int64_t quantity)
      : symbol_(std::move(symbol)), price_(price), quantity_(quantity) {
    detail::require_symbol(symbol_);
    detail::require_price(price_);
    detail::require_quantity(quantity_);
  }

  const std::string& symbol() const { return symbol_; }
  double price() const { return price_; }
  std::int64_t quantity() const { return quantity_; }

 private:
  std::string symbol_;
  double price_;
  std::int64_t quantity_;
};

class Order final {
 public:
  Order(std::string symbol, Side side, std::int64_t quantity)
      : symbol_(std::move(symbol)), side_(side), quantity_(quantity) {
    detail::require_symbol(symbol_);
    detail::require_quantity(quantity_);
  }

  const std::string& symbol() const { return symbol_; }
  Side side() const { return side_; }
  std::int64_t quantity() const { return quantity_; }

 private:
  std::string symbol_;
  Side side_;
  std::int64_t quantity_;
};

struct PortfolioSnapshot final {
  std::string symbol;
  std::int64_t quantity{};
  double cash{};
  double equity{};
};

}  // namespace quant::ch09

#endif
