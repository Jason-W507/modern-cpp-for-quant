#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <optional>
#include <thread>
#include <utility>

template <typename T, std::size_t Capacity>
class SpscRing final {
  static_assert(Capacity > 0);

 public:
  bool try_push(T value) {
    const std::size_t head = head_.load(std::memory_order_relaxed);
    const std::size_t next = increment(head);
    if (next == tail_.load(std::memory_order_acquire)) {
      return false;
    }
    slots_[head].emplace(std::move(value));
    head_.store(next, std::memory_order_release);
    return true;
  }

  std::optional<T> try_pop() {
    const std::size_t tail = tail_.load(std::memory_order_relaxed);
    if (tail == head_.load(std::memory_order_acquire)) {
      return std::nullopt;
    }
    std::optional<T> result{std::move(slots_[tail])};
    slots_[tail].reset();
    tail_.store(increment(tail), std::memory_order_release);
    return result;
  }

 private:
  static constexpr std::size_t storage_size = Capacity + 1;
  static constexpr std::size_t increment(std::size_t value) {
    return (value + 1) % storage_size;
  }

  std::array<std::optional<T>, storage_size> slots_{};
  alignas(64) std::atomic<std::size_t> head_{};
  alignas(64) std::atomic<std::size_t> tail_{};
};

int main() {
  constexpr std::int64_t items = 10'000;
  SpscRing<std::int64_t, 64> queue;
  std::atomic<bool> start{false};
  std::int64_t consumed = 0;
  std::int64_t checksum = 0;

  std::jthread producer{[&] {
    while (!start.load(std::memory_order_acquire)) {
      std::this_thread::yield();
    }
    for (std::int64_t value = 1; value <= items; ++value) {
      while (!queue.try_push(value)) {
        std::this_thread::yield();
      }
    }
  }};
  std::jthread consumer{[&] {
    start.store(true, std::memory_order_release);
    while (consumed < items) {
      if (const auto value = queue.try_pop()) {
        ++consumed;
        checksum += *value;
      } else {
        std::this_thread::yield();
      }
    }
  }};
  producer.join();
  consumer.join();

  constexpr std::int64_t expected = items * (items + 1) / 2;
  if (consumed != items || checksum != expected) {
    std::cerr << "SPSC oracle mismatch\n";
    return 2;
  }
  std::cout << "spsc-ok produced=" << items << " consumed=" << consumed
            << " checksum=" << checksum << '\n';
}
