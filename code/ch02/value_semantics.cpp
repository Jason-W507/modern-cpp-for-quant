#include <iostream>
#include <string>

#include "quant/types.hpp"

struct Position final {
  std::string symbol;
  std::int64_t quantity{};
  double mark_price{};

  [[nodiscard]] double notional() const {
    return static_cast<double>(quantity) * mark_price;
  }
};

void apply_fill(Position& position, const quant::Fill& fill) {
  position.quantity += quant::signed_quantity(fill.side, fill.quantity);
  position.mark_price = fill.price;
}

int main() {
  Position position{"AAPL", 100, 187.0};
  const quant::Fill fill{{}, "AAPL", quant::Side::buy, 20, 188.5};
  apply_fill(position, fill);
  std::cout << position.symbol << " qty=" << position.quantity
            << " notional=" << position.notional() << '\n';
}
