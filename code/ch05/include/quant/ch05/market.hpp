#ifndef QUANT_CH05_MARKET_HPP
#define QUANT_CH05_MARKET_HPP

#include <cstddef>
#include <string>
#include <vector>

namespace quant::ch05 {

enum class Trend { down, flat, up };

struct MarketSummary {
  std::string symbol;
  std::size_t rows{};
  double first_price{};
  double last_price{};
  double return_percent{};
  double low{};
  double high{};
  Trend trend{Trend::flat};
  int total_quantity{};
};

class MarketQuote {
 public:
  MarketQuote(std::string symbol, double price, int quantity);

  const std::string& symbol() const;
  double price() const;
  int quantity() const;

 private:
  std::string symbol_;
  double price_{};
  int quantity_{};
};

class MarketAnalyzer {
 public:
  MarketAnalyzer(std::string symbol);

  void add(const MarketQuote& quote);
  MarketSummary summary() const;

 private:
  std::string symbol_;
  std::vector<double> prices_;
  int total_quantity_{};
};

const char* trend_name(Trend trend);

}  // namespace quant::ch05

#endif
