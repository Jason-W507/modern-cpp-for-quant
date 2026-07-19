#include "quant/ch15/bounded_queue.hpp"

#include <iostream>
#include <stop_token>

int main() {
  using quant::ch15::BoundedQueue;
  using quant::ch15::PopStatus;
  using quant::ch15::PushStatus;

  BoundedQueue<int> queue{2};
  const PushStatus first = queue.try_push(10);
  const PushStatus second = queue.try_push(20);
  const PushStatus full = queue.try_push(30);

  std::stop_source running;
  const auto first_pop = queue.wait_pop(running.get_token());
  const auto second_pop = queue.wait_pop(running.get_token());
  queue.close();
  const PushStatus after_close = queue.try_push(40);
  const auto drained = queue.wait_pop(running.get_token());

  const bool accepted = first == PushStatus::accepted &&
                        second == PushStatus::accepted;
  const bool full_reported = full == PushStatus::full;
  const bool values_match = first_pop.status == PopStatus::value &&
                            second_pop.status == PopStatus::value &&
                            first_pop.value == 10 && second_pop.value == 20;
  const bool closed = after_close == PushStatus::closed &&
                      drained.status == PopStatus::closed;

  std::cout << "queue-ok capacity=" << queue.capacity()
            << " accepted=" << (accepted ? 2 : 0)
            << " full=" << (full_reported ? 1 : 0)
            << " drained-sum="
            << (first_pop.value.value() + second_pop.value.value())
            << " closed=" << std::boolalpha << closed << '\n';
  return accepted && full_reported && values_match && closed ? 0 : 2;
}
