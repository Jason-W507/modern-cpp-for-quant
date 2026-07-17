#include <cstdint>
#include <iostream>

int main() {
  std::int64_t symbol_id{101};
  int side_code{1};
  double price{100.5};
  std::int64_t quantity{10};

  bool is_buy{};
  switch (side_code) {
    case 1:
      is_buy = true;
      break;
    case 2:
      is_buy = false;
      break;
    default:
      is_buy = false;
      break;
  }

  bool valid_row = symbol_id > 0 && price > 0.0 && quantity > 0;
  if (is_buy && valid_row) {
    std::cout << "selected\n";
  } else {
    std::cout << "rejected\n";
  }
}
