#include "quant/ch07/pipeline.hpp"

#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <stdexcept>

namespace quant::ch07 {

TradeSummary summarize_trades(const std::vector<Trade>& trades,
                              const std::string& symbol) {
  auto selected = trades | std::views::filter([&symbol](const Trade& trade) {
                    return trade.symbol == symbol;
                  });

  std::vector<Trade> sorted;
  std::ranges::copy(selected, std::back_inserter(sorted));
  if (sorted.empty()) {
    throw std::logic_error{"no trades for symbol " + symbol};
  }
  std::ranges::sort(sorted, [](const Trade& left, const Trade& right) {
    return left.price < right.price;
  });

  const auto first_sell =
      std::ranges::find_if(selected, [](const Trade& trade) {
        return trade.side == Side::sell;
      });
  if (first_sell == selected.end()) {
    throw std::logic_error{"no sell trade for symbol " + symbol};
  }

  auto notionals = selected | std::views::transform([](const Trade& trade) {
                     return trade.price * static_cast<double>(trade.quantity);
                   });
  const double gross{
      std::accumulate(notionals.begin(), notionals.end(), 0.0)};

  std::map<std::string, int> positions;
  for (const Trade& trade : trades) {
    int signed_quantity{trade.quantity};
    if (trade.side == Side::sell) {
      signed_quantity = -trade.quantity;
    }
    positions[trade.symbol] += signed_quantity;
  }

  return TradeSummary{symbol,          sorted.size(), first_sell->price,
                      sorted.front().price, sorted.back().price, gross,
                      positions.at(symbol)};
}

}  // namespace quant::ch07
