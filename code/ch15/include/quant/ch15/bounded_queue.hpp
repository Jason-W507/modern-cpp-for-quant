#ifndef QUANT_CH15_BOUNDED_QUEUE_HPP
#define QUANT_CH15_BOUNDED_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <stop_token>
#include <utility>

namespace quant::ch15 {

enum class PushStatus { accepted, full, closed, stopped };
enum class PopStatus { value, closed, stopped };

template <typename T>
struct PopResult final {
  PopStatus status;
  std::optional<T> value;
};

template <typename T>
class BoundedQueue final {
 public:
  explicit BoundedQueue(std::size_t capacity) : capacity_{capacity} {
    if (capacity_ == 0) {
      throw std::invalid_argument{"queue capacity must be positive"};
    }
  }

  BoundedQueue(const BoundedQueue&) = delete;
  BoundedQueue& operator=(const BoundedQueue&) = delete;

  PushStatus try_push(T value) {
    std::lock_guard lock{mutex_};
    if (closed_) {
      return PushStatus::closed;
    }
    if (items_.size() == capacity_) {
      return PushStatus::full;
    }
    items_.push_back(std::move(value));
    not_empty_.notify_one();
    return PushStatus::accepted;
  }

  PushStatus wait_push(T value, std::stop_token stop) {
    std::unique_lock lock{mutex_};
    if (stop.stop_requested()) {
      return PushStatus::stopped;
    }
    const bool ready = not_full_.wait(lock, stop, [this] {
      return closed_ || items_.size() < capacity_;
    });
    if (!ready || stop.stop_requested()) {
      return PushStatus::stopped;
    }
    if (closed_) {
      return PushStatus::closed;
    }
    items_.push_back(std::move(value));
    not_empty_.notify_one();
    return PushStatus::accepted;
  }

  PopResult<T> wait_pop(std::stop_token stop) {
    std::unique_lock lock{mutex_};
    if (stop.stop_requested()) {
      return {PopStatus::stopped, std::nullopt};
    }
    const bool ready = not_empty_.wait(lock, stop, [this] {
      return closed_ || !items_.empty();
    });
    if (!ready || stop.stop_requested()) {
      return {PopStatus::stopped, std::nullopt};
    }
    if (items_.empty()) {
      return {PopStatus::closed, std::nullopt};
    }
    T value = std::move(items_.front());
    items_.pop_front();
    not_full_.notify_one();
    return {PopStatus::value, std::move(value)};
  }

  void close() {
    {
      std::lock_guard lock{mutex_};
      closed_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  [[nodiscard]] std::size_t capacity() const noexcept { return capacity_; }

  [[nodiscard]] std::size_t size() const {
    std::lock_guard lock{mutex_};
    return items_.size();
  }

  [[nodiscard]] bool closed() const {
    std::lock_guard lock{mutex_};
    return closed_;
  }

 private:
  const std::size_t capacity_;
  mutable std::mutex mutex_;
  std::condition_variable_any not_empty_;
  std::condition_variable_any not_full_;
  std::deque<T> items_;
  bool closed_{false};
};

}  // namespace quant::ch15

#endif
