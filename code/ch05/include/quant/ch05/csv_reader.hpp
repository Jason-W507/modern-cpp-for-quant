#ifndef QUANT_CH05_CSV_READER_HPP
#define QUANT_CH05_CSV_READER_HPP

#include <istream>
#include <vector>

#include "quant/ch05/market.hpp"

namespace quant::ch05 {

std::vector<MarketQuote> read_market_csv(std::istream& input);

}  // namespace quant::ch05

#endif
