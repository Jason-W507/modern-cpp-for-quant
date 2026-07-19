#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>

struct Quote final {
  double price;
  std::int64_t quantity;
  char venue;
};

int main() {
  constexpr std::size_t field_bytes =
      sizeof(double) + sizeof(std::int64_t) + sizeof(char);
  const bool valid = std::is_standard_layout_v<Quote> &&
                     sizeof(Quote) >= field_bytes &&
                     offsetof(Quote, price) < offsetof(Quote, quantity) &&
                     offsetof(Quote, quantity) < offsetof(Quote, venue);
  if (!valid) {
    std::cerr << "layout invariants were not satisfied\n";
    return 1;
  }

  std::cout << "layout-facts-ok sizeof-quote=" << sizeof(Quote)
            << " alignof-quote=" << alignof(Quote)
            << " field-bytes=" << field_bytes
            << " padding-bytes=" << sizeof(Quote) - field_bytes
            << " offsets=" << offsetof(Quote, price) << ','
            << offsetof(Quote, quantity) << ',' << offsetof(Quote, venue)
            << '\n';
}
