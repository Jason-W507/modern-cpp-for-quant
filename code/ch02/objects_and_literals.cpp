#include <cstdint>
#include <iostream>

int main() {
  int side_code{1};
  double price{100.5};
  std::int64_t quantity{10};
  bool is_buy{true};
  char venue{'X'};

  int original{10};
  int copied{original};
  original = 11;

  std::cout << "side=" << side_code << " price=" << price
            << " quantity=" << quantity << " buy=" << is_buy
            << " venue=" << venue << " changed=" << original
            << " copied=" << copied << '\n';
}
