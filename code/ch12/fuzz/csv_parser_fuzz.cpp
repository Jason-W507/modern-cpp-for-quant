#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "quant/csv.hpp"

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data,
                                      std::size_t size) {
  std::string bytes{reinterpret_cast<const char*>(data), size};
  std::istringstream input{bytes};
  const auto result = quant::read_market_events(input);
  if (result.has_value()) {
    for (const auto& event : result.events) {
      if (event.symbol.empty() || event.price <= 0.0 || event.quantity <= 0) {
        __builtin_trap();
      }
    }
  }
  return 0;
}
