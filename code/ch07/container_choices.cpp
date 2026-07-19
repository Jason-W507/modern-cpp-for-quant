#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "quant/ch07/trade.hpp"

int main() {
  const std::vector<quant::ch07::Trade> trades{
      {"AAPL", quant::ch07::Side::buy, 10, 100.0},
      {"MSFT", quant::ch07::Side::buy, 4, 200.0},
      {"AAPL", quant::ch07::Side::sell, 5, 102.0},
      {"AAPL", quant::ch07::Side::buy, 10, 101.0},
  };

  std::map<std::string, int> positions;
  for (const auto& trade : trades) {
    int signed_quantity{trade.quantity};
    if (trade.side == quant::ch07::Side::sell) {
      signed_quantity = -trade.quantity;
    }
    positions[trade.symbol] += signed_quantity;
  }

  const auto first_sell =
      std::ranges::find_if(trades, [](const quant::ch07::Trade& trade) {
        return trade.side == quant::ch07::Side::sell;
      });
  auto sorted{trades};
  std::ranges::sort(
      sorted, [](const quant::ch07::Trade& left,
                 const quant::ch07::Trade& right) {
        return left.price < right.price;
      });

  std::cout << std::fixed << std::setprecision(2)
            << "rows=" << trades.size() << " symbols=" << positions.size()
            << " aapl_position=" << positions.at("AAPL")
            << " first_sell=" << first_sell->symbol << '@' << first_sell->price
            << " lowest=" << sorted.front().symbol << '@'
            << sorted.front().price << '\n';
}
