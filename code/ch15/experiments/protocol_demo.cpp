#include "quant/ch15/bounded_queue.hpp"

#include <future>
#include <iostream>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <thread>

int main() {
  using quant::ch15::BoundedQueue;
  using quant::ch15::PopStatus;

  BoundedQueue<int> consumer_queue{1};
  std::promise<void> consumer_started_signal;
  std::future<void> consumer_started = consumer_started_signal.get_future();
  std::promise<PopStatus> consumer_stop_signal;
  std::future<PopStatus> consumer_stopped = consumer_stop_signal.get_future();

  std::jthread consumer{[&](std::stop_token stop) {
    consumer_started_signal.set_value();
    consumer_stop_signal.set_value(consumer_queue.wait_pop(stop).status);
  }};
  consumer_started.get();
  consumer.request_stop();
  const bool consumer_stop = consumer_stopped.get() == PopStatus::stopped;
  consumer.join();

  BoundedQueue<int> producer_queue{1};
  (void)producer_queue.try_push(7);
  std::promise<void> producer_started_signal;
  std::future<void> producer_started = producer_started_signal.get_future();
  std::promise<quant::ch15::PushStatus> producer_stop_signal;
  std::future<quant::ch15::PushStatus> producer_stopped =
      producer_stop_signal.get_future();

  std::jthread producer{[&](std::stop_token stop) {
    producer_started_signal.set_value();
    producer_stop_signal.set_value(producer_queue.wait_push(8, stop));
  }};
  producer_started.get();
  producer.request_stop();
  const bool producer_stop =
      producer_stopped.get() == quant::ch15::PushStatus::stopped;
  producer.join();
  const bool started_observed = true;

  auto failed_task = std::async(std::launch::async, [] {
    throw std::runtime_error{"negative quantity"};
  });
  bool exception_propagated = false;
  try {
    failed_task.get();
  } catch (const std::runtime_error& error) {
    exception_propagated = std::string{error.what()} == "negative quantity";
  }

  consumer_queue.close();
  producer_queue.close();
  const bool queues_closed =
      consumer_queue.closed() && producer_queue.closed();
  const bool valid = started_observed && consumer_stop && producer_stop &&
                     exception_propagated && queues_closed;
  std::cout << "protocol-ok started=" << std::boolalpha << started_observed
            << " consumer-stop=" << consumer_stop
            << " producer-stop=" << producer_stop
            << " exception-propagated=" << exception_propagated
            << " queues-closed=" << queues_closed << '\n';
  return valid ? 0 : 2;
}
