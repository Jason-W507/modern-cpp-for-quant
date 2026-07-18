#include <vector>

int main() {
  std::vector<int> values;
  values.reserve(1);
  values.push_back(7);

  const auto stale = values.begin();
  const auto previous_capacity = values.capacity();
  while (values.capacity() == previous_capacity) {
    values.push_back(8);
  }

  return *stale;
}
