#include <barrier>
#include <iostream>
#include <thread>

int main() {
  constexpr int increments_per_thread = 100'000;
  int shared_counter = 0;
  std::barrier start_line{3};

  const auto increment = [&] {
    start_line.arrive_and_wait();
    for (int index = 0; index < increments_per_thread; ++index) {
      ++shared_counter;  // Intentional data race: two unsynchronized writers.
    }
  };

  std::jthread first{increment};
  std::jthread second{increment};
  start_line.arrive_and_wait();
  first.join();
  second.join();

  std::cout << "invalid-racy-counter=" << shared_counter << '\n';
}
