#include "quant/ch11/market_summary.hpp"

#ifndef CH11_BUILDING_LIBRARY
#error "CH11_BUILDING_LIBRARY must remain private to the library target"
#endif

#include <stdexcept>

namespace quant::ch11 {

MarketSummary summarize(std::span<const MarketRow> rows) {
  if (rows.empty()) {
    throw std::invalid_argument{"market rows must not be empty"};
  }

  MarketSummary result{rows.size(), 0, 0.0, 0.0};
  for (const auto& row : rows) {
    if (row.price <= 0.0 || row.quantity <= 0) {
      throw std::invalid_argument{"price and quantity must be positive"};
    }
    result.quantity += row.quantity;
    result.notional += row.price * static_cast<double>(row.quantity);
  }
  result.vwap = result.notional / static_cast<double>(result.quantity);
  return result;
}

}  // namespace quant::ch11
