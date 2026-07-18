#include <iostream>
#include <string>

enum class Trend { down, flat, up };

struct QuoteRecord {
  std::string symbol;
  double price{};
  int quantity{};
};

const char* trend_name(const Trend trend) {
  switch (trend) {
    case Trend::down:
      return "down";
    case Trend::flat:
      return "flat";
    case Trend::up:
      return "up";
  }
  return "unknown";
}

int main() {
  QuoteRecord quote{"AAPL", 100.0, 10};
  quote.price = -1.0;
  const Trend trend{Trend::up};
  std::cout << "symbol=" << quote.symbol << " price=" << quote.price
            << " quantity=" << quote.quantity
            << " trend=" << trend_name(trend) << '\n';
}
