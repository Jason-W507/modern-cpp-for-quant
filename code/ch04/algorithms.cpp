#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

bool is_valid_price(const double price) {
  return price > 0.0;
}

int main() {
  const std::vector<double> prices{101.0, -1.0, 99.0, 100.0};

  int manual_valid{0};
  for (const double price : prices) {
    if (is_valid_price(price)) {
      ++manual_valid;
    }
  }

  const auto named_valid{
      std::count_if(prices.begin(), prices.end(), is_valid_price)};
  const auto lambda_valid{std::count_if(
      prices.begin(), prices.end(),
      [](const double price) { return price > 0.0; })};

  std::vector<double> valid_prices;
  std::copy_if(prices.begin(), prices.end(),
               std::back_inserter(valid_prices), is_valid_price);
  std::sort(valid_prices.begin(), valid_prices.end());

  std::vector<double> notionals(valid_prices.size());
  std::transform(valid_prices.begin(), valid_prices.end(), notionals.begin(),
                 [](const double price) { return price * 10.0; });
  const double total_notional{
      std::accumulate(notionals.begin(), notionals.end(), 0.0)};

  std::cout << "manual_valid=" << manual_valid
            << " named_valid=" << named_valid
            << " lambda_valid=" << lambda_valid
            << " first=" << valid_prices[0]
            << " total_notional=" << total_notional << '\n';
}
