#include <iostream>

int main() {
  int selected{};
  for (int row{}; row < 5; ++row) {
    int remainder = row % 2;
    if (remainder == 0) {
      ++selected;
    }
  }

  std::cout << "selected=" << selected << '\n';
}
