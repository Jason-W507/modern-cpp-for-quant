#include <iostream>
#include <vector>

int main() {
  std::vector<int> values;
  values.reserve(1);
  values.push_back(7);

  auto first = values.begin();
  const auto previous_capacity = values.capacity();
  while (values.capacity() == previous_capacity) {
    values.push_back(8);
  }

  const bool reallocated{values.capacity() != previous_capacity};
  first = values.begin();
  auto second = first;
  ++second;
  const bool has_multiple{second != values.end()};

  std::cout << "reallocated=" << reallocated << " reacquired=" << *first
            << " has_multiple=" << has_multiple << '\n';
}
