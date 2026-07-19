#ifndef QUANT_CH07_PIPELINE_HPP
#define QUANT_CH07_PIPELINE_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "quant/ch07/trade.hpp"

namespace quant::ch07 {

struct TradeSummary {
  std::string symbol;
  std::size_t trades{};
  double first_sell{};
  double low{};
  double high{};
  double gross{};
  int net_quantity{};
};

TradeSummary summarize_trades(const std::vector<Trade>& trades,
                              const std::string& symbol);

}  // namespace quant::ch07

#endif
