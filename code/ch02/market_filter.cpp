#include <cstdint>
#include <iostream>

int main() {
  int row_count{};
  if (!(std::cin >> row_count) || row_count < 0) {
    std::cerr << "invalid row count\n";
    return 1;
  }

  int selected{};
  int rejected{};
  double buy_notional{};

  for (int row{}; row < row_count; ++row) {
    std::int64_t symbol_id{};
    int side_code{};
    double price{};
    std::int64_t quantity{};
    if (!(std::cin >> symbol_id >> side_code >> price >> quantity)) {
      std::cerr << "invalid market row " << row << '\n';
      return 1;
    }

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
      auto row_notional = price * static_cast<double>(quantity);
      buy_notional += row_notional;
      ++selected;
    } else {
      ++rejected;
    }
  }

  double average{};
  if (selected > 0) {
    average = buy_notional / static_cast<double>(selected);
  }

  std::cout << "selected=" << selected << " rejected=" << rejected
            << " buy_notional=" << buy_notional << " average=" << average
            << '\n';
}
