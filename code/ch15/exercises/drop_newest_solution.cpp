#include "quant/ch15/bounded_queue.hpp"

#include <array>
#include <iostream>
#include <stop_token>

int main() {
  using quant::ch15::BoundedQueue;
  using quant::ch15::PushStatus;

  BoundedQueue<int> queue{2};
  const std::array offered{10, 20, 30, 40, 50};
  int accepted = 0;
  int dropped = 0;
  for (const int value : offered) {
    const PushStatus status = queue.try_push(value);
    accepted += status == PushStatus::accepted ? 1 : 0;
    dropped += status == PushStatus::full ? 1 : 0;
  }

  std::stop_source running;
  const auto first = queue.wait_pop(running.get_token());
  const auto second = queue.wait_pop(running.get_token());
  queue.close();
  const int drained_sum = first.value.value() + second.value.value();
  const bool valid = accepted == 2 && dropped == 3 && drained_sum == 30;

  std::cout << "drop-newest-ok offered=" << offered.size()
            << " accepted=" << accepted << " dropped=" << dropped
            << " drained-sum=" << drained_sum << '\n';
  return valid ? 0 : 2;
}
