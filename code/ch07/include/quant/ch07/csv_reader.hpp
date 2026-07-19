#ifndef QUANT_CH07_CSV_READER_HPP
#define QUANT_CH07_CSV_READER_HPP

#include <iosfwd>
#include <vector>

#include "quant/ch07/trade.hpp"

namespace quant::ch07 {

std::vector<Trade> read_trades(std::istream& input);

}  // namespace quant::ch07

#endif
