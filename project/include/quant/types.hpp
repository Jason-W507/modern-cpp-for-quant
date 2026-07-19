#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace quant {

using Timestamp = std::chrono::sys_time<std::chrono::milliseconds>;

enum class Side : std::uint8_t { buy, sell };

struct MarketEvent final {
  Timestamp timestamp{};
  std::string symbol;
  double price{};
  std::int64_t quantity{};
};

struct OrderIntent final {
  std::string symbol;
  Side side{Side::buy};
  std::int64_t quantity{};
};

struct Fill final {
  Timestamp timestamp{};
  std::string symbol;
  Side side{Side::buy};
  std::int64_t quantity{};
  double price{};
  double fee{};
};

struct PortfolioSnapshot final {
  std::string symbol;
  std::int64_t quantity{};
  double cash{};
  double equity{};
};

[[nodiscard]] constexpr std::int64_t signed_quantity(Side side,
                                                      std::int64_t quantity) {
  return side == Side::buy ? quantity : -quantity;
}

}  // namespace quant
