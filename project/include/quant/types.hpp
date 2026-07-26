#pragma once

#include <chrono>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace quant {

using Timestamp = std::chrono::sys_time<std::chrono::milliseconds>;

enum class Side : std::uint8_t { buy, sell };

[[nodiscard]] constexpr bool is_valid_side(Side side) noexcept {
  switch (side) {
    case Side::buy:
    case Side::sell:
      return true;
  }
  return false;
}

namespace detail {

inline void validate_nonempty_symbol(const std::string& symbol,
                                     const char* owner) {
  if (symbol.empty()) {
    throw std::invalid_argument{std::string{owner} +
                                " requires a nonempty symbol"};
  }
}

inline void validate_positive_finite(double value, const char* field) {
  if (!std::isfinite(value) || value <= 0.0) {
    throw std::invalid_argument{std::string{field} +
                                " must be finite and positive"};
  }
}

inline void validate_positive_quantity(std::int64_t value,
                                       const char* field) {
  if (value <= 0) {
    throw std::invalid_argument{std::string{field} + " must be positive"};
  }
}

inline void validate_side(Side side, const char* owner) {
  if (!is_valid_side(side)) {
    throw std::invalid_argument{std::string{owner} + " requires buy or sell"};
  }
}

}  // namespace detail

class MarketEvent final {
 public:
  MarketEvent(Timestamp timestamp, std::string symbol, double price,
              std::int64_t quantity)
      : timestamp_(timestamp),
        symbol_(std::move(symbol)),
        price_(price),
        quantity_(quantity) {
    detail::validate_nonempty_symbol(symbol_, "market event");
    detail::validate_positive_finite(price_, "market event price");
    detail::validate_positive_quantity(quantity_, "market event quantity");
  }

  [[nodiscard]] Timestamp timestamp() const noexcept { return timestamp_; }
  [[nodiscard]] const std::string& symbol() const noexcept { return symbol_; }
  [[nodiscard]] double price() const noexcept { return price_; }
  [[nodiscard]] std::int64_t quantity() const noexcept { return quantity_; }

 private:
  Timestamp timestamp_;
  std::string symbol_;
  double price_;
  std::int64_t quantity_;
};

class OrderIntent final {
 public:
  OrderIntent(std::string symbol, Side side, std::int64_t quantity)
      : symbol_(std::move(symbol)), side_(side), quantity_(quantity) {
    detail::validate_nonempty_symbol(symbol_, "order intent");
    detail::validate_side(side_, "order intent");
    detail::validate_positive_quantity(quantity_, "order intent quantity");
  }

  [[nodiscard]] const std::string& symbol() const noexcept { return symbol_; }
  [[nodiscard]] Side side() const noexcept { return side_; }
  [[nodiscard]] std::int64_t quantity() const noexcept { return quantity_; }

 private:
  std::string symbol_;
  Side side_;
  std::int64_t quantity_;
};

class Fill final {
 public:
  Fill(Timestamp timestamp, std::string symbol, Side side,
       std::int64_t quantity, double price, double fee)
      : timestamp_(timestamp),
        symbol_(std::move(symbol)),
        side_(side),
        quantity_(quantity),
        price_(price),
        fee_(fee) {
    detail::validate_nonempty_symbol(symbol_, "fill");
    detail::validate_side(side_, "fill");
    detail::validate_positive_quantity(quantity_, "fill quantity");
    detail::validate_positive_finite(price_, "fill price");
    if (!std::isfinite(fee_) || fee_ < 0.0) {
      throw std::invalid_argument{"fill fee must be finite and nonnegative"};
    }
  }

  [[nodiscard]] Timestamp timestamp() const noexcept { return timestamp_; }
  [[nodiscard]] const std::string& symbol() const noexcept { return symbol_; }
  [[nodiscard]] Side side() const noexcept { return side_; }
  [[nodiscard]] std::int64_t quantity() const noexcept { return quantity_; }
  [[nodiscard]] double price() const noexcept { return price_; }
  [[nodiscard]] double fee() const noexcept { return fee_; }

 private:
  Timestamp timestamp_;
  std::string symbol_;
  Side side_;
  std::int64_t quantity_;
  double price_;
  double fee_;
};

struct PortfolioSnapshot final {
  std::string symbol;
  std::int64_t quantity{};
  double cash{};
  double equity{};
};

}  // namespace quant
