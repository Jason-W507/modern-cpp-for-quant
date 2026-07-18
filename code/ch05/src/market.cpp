#include "quant/ch05/market.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace quant::ch05 {

MarketQuote::MarketQuote(std::string symbol, const double price,
                         const int quantity)
    : symbol_{symbol}, price_{price}, quantity_{quantity} {
  if (symbol_.empty()) {
    throw std::invalid_argument{"symbol must not be empty"};
  }
  if (!std::isfinite(price_) || price_ <= 0.0) {
    throw std::invalid_argument{"price must be finite and positive"};
  }
  if (quantity_ <= 0) {
    throw std::invalid_argument{"quantity must be positive"};
  }
}

const std::string& MarketQuote::symbol() const { return symbol_; }

double MarketQuote::price() const { return price_; }

int MarketQuote::quantity() const { return quantity_; }

MarketAnalyzer::MarketAnalyzer(std::string symbol) : symbol_{symbol} {
  if (symbol_.empty()) {
    throw std::invalid_argument{"analyzer symbol must not be empty"};
  }
}

void MarketAnalyzer::add(const MarketQuote& quote) {
  if (quote.symbol() != symbol_) {
    throw std::invalid_argument{"symbol does not match analyzer"};
  }
  this->prices_.push_back(quote.price());
  this->total_quantity_ += quote.quantity();
}

MarketSummary MarketAnalyzer::summary() const {
  if (prices_.empty()) {
    throw std::logic_error{"no rows for symbol " + symbol_};
  }
  const auto bounds{std::minmax_element(prices_.begin(), prices_.end())};
  const double first{prices_.front()};
  const double last{prices_.back()};
  Trend trend{Trend::flat};
  if (last > first) {
    trend = Trend::up;
  } else if (last < first) {
    trend = Trend::down;
  }
  return MarketSummary{symbol_, prices_.size(), first, last,
                       (last / first - 1.0) * 100.0, *bounds.first,
                       *bounds.second, trend, total_quantity_};
}

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

}  // namespace quant::ch05
