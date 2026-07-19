#ifndef QUANT_CH11_MARKET_SUMMARY_HPP
#define QUANT_CH11_MARKET_SUMMARY_HPP

#include <cstddef>
#include <cstdint>
#include <span>

namespace quant::ch11 {

struct MarketRow {
  double price;
  std::int64_t quantity;
};

struct MarketSummary {
  std::size_t rows;
  std::int64_t quantity;
  double notional;
  double vwap;
};

[[nodiscard]] MarketSummary summarize(std::span<const MarketRow> rows);

}  // namespace quant::ch11

#endif
