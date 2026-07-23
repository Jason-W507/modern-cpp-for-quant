#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

namespace quant::capstone {

template <typename T, std::size_t Capacity>
class SpscRing final {
  static_assert(Capacity > 0);

 public:
  bool try_push(T&& value) {
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

  [[nodiscard]] bool empty() const {
    return tail_.load(std::memory_order_acquire) ==
           head_.load(std::memory_order_acquire);
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

}  // namespace quant::capstone
