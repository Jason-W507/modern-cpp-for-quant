#ifndef QUANT_CH08_EVENT_READER_HPP
#define QUANT_CH08_EVENT_READER_HPP

#include <string>
#include <vector>

#include "quant/ch08/domain.hpp"

namespace quant::ch08 {

std::vector<MarketEvent> read_events(const std::string& path);

}  // namespace quant::ch08

#endif
