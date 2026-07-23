#include "quant/capstone/market_replay.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data,
                                      std::size_t size) {
  quant::capstone::MarketReplay replay;
  replay.apply(std::span<const std::uint8_t>{data, size});
  return 0;
}
