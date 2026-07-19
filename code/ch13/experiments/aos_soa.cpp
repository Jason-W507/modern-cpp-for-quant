#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

struct Quote final {
  double price;
  std::int64_t quantity;
  std::int64_t timestamp;
  char venue;
};

int main() {
  const std::vector<Quote> rows{
      {99.0, 2, 1, 'X'}, {100.0, 3, 2, 'X'}, {101.5, 4, 3, 'Y'},
      {98.0, 5, 4, 'Y'}, {102.0, 1, 5, 'X'}};
  const std::vector<double> prices{99.0, 100.0, 101.5, 98.0, 102.0};
  const std::vector<std::int64_t> quantities{2, 3, 4, 5, 1};

  double aos_checksum = 0.0;
  for (const Quote& row : rows) {
    aos_checksum += row.price * static_cast<double>(row.quantity);
  }

  double soa_checksum = 0.0;
  for (std::size_t index = 0; index < prices.size(); ++index) {
    soa_checksum += prices[index] * static_cast<double>(quantities[index]);
  }

  if (aos_checksum != 1'496.0 || soa_checksum != aos_checksum) {
    std::cerr << "layout variants changed the calculation\n";
    return 1;
  }

  const std::size_t modeled_aos_bytes = rows.size() * sizeof(Quote);
  const std::size_t modeled_soa_hot_bytes =
      rows.size() * (sizeof(double) + sizeof(std::int64_t));
  std::cout << std::fixed << std::setprecision(2)
            << "aos-soa-ok rows=" << rows.size()
            << " checksum=" << aos_checksum
            << " modeled-aos-bytes=" << modeled_aos_bytes
            << " modeled-soa-hot-bytes=" << modeled_soa_hot_bytes << '\n';
}
