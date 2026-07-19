#ifndef QUANT_CH07_TRADE_HPP
#define QUANT_CH07_TRADE_HPP

#include <string>

namespace quant::ch07 {

enum class Side { buy, sell };

struct Trade {
  std::string symbol;
  Side side{Side::buy};
  int quantity{};
  double price{};
};

}  // namespace quant::ch07

#endif
