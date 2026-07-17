#include <cstdint>
#include <iostream>

int main() {
  double price{100.5};
  std::int64_t quantity{10};
  int selected{2};

  auto notional = price * static_cast<double>(quantity);
  double average = notional / static_cast<double>(selected);
  int integer_division = 5 / 2;

  std::cout << "notional=" << notional << " average=" << average
            << " integer_division=" << integer_division << '\n';
}
