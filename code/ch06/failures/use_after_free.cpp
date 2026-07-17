#include <iostream>

int main() {
  int* quantity{new int{10}};
  delete quantity;
  std::cout << *quantity << '\n';  // intentional heap-use-after-free
}
