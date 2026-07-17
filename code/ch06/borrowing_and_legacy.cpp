#include <iostream>

void add_half_tick(double& price) { price += 0.5; }

int main() {
  double price{100.0};
  double& alias{price};
  add_half_tick(alias);

  const double* observer{&price};
  std::cout << "price=" << price << " alias=" << alias
            << " observed=" << *observer;

  observer = nullptr;
  int stack_quotes[3]{100, 101, 102};
  int* heap_quantity{new int{10}};
  std::cout << " missing=" << (observer == nullptr)
            << " stack_last=" << stack_quotes[2]
            << " heap_quantity=" << *heap_quantity;

  delete heap_quantity;
  heap_quantity = nullptr;
  std::cout << " released=" << (heap_quantity == nullptr) << '\n';
}
