#include "quant/ch09/domain.hpp"

#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace quant::ch09::detail {

void require_symbol(const std::string& symbol) {
  if (symbol.empty()) {
    throw std::invalid_argument{"symbol must not be empty"};
  }
}

void require_price(double price) {
  if (!std::isfinite(price) || price <= 0.0) {
    throw std::invalid_argument{"price must be positive and finite"};
  }
}

void require_quantity(std::int64_t quantity) {
  if (quantity <= 0) {
    throw std::invalid_argument{"quantity must be positive"};
  }
}

}  // namespace quant::ch09::detail
