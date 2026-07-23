#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

std::vector<int> top_k(const std::vector<int>& values, std::size_t k) {
  std::priority_queue<int, std::vector<int>, std::greater<>> heap;
  for (const int value : values) {
    heap.push(value);
    if (heap.size() > k) {
      heap.pop();
    }
  }
  std::vector<int> result;
  while (!heap.empty()) {
    result.push_back(heap.top());
    heap.pop();
  }
  std::ranges::sort(result, std::greater<>{});
  return result;
}

std::size_t lower_bound_index(const std::vector<int>& values, int needle) {
  std::size_t first = 0;
  std::size_t last = values.size();
  while (first < last) {
    const std::size_t middle = first + (last - first) / 2;
    if (values[middle] < needle) {
      first = middle + 1;
    } else {
      last = middle;
    }
  }
  return first;
}

class LruCache final {
 public:
  explicit LruCache(std::size_t capacity) : capacity_{capacity} {
    if (capacity == 0) {
      throw std::invalid_argument{"LRU capacity must be positive"};
    }
  }

  void put(int key, int value) {
    if (const auto found = index_.find(key); found != index_.end()) {
      entries_.erase(found->second);
      index_.erase(found);
    }
    entries_.push_front({key, value});
    index_[key] = entries_.begin();
    if (entries_.size() > capacity_) {
      index_.erase(entries_.back().first);
      entries_.pop_back();
    }
  }

  std::optional<int> get(int key) {
    const auto found = index_.find(key);
    if (found == index_.end()) {
      return std::nullopt;
    }
    const auto entry = *found->second;
    entries_.erase(found->second);
    entries_.push_front(entry);
    index_[key] = entries_.begin();
    return entry.second;
  }

 private:
  using Entry = std::pair<int, int>;
  using Iterator = std::list<Entry>::iterator;
  std::size_t capacity_;
  std::list<Entry> entries_;
  std::unordered_map<int, Iterator> index_;
};

std::vector<int> sliding_maximum(const std::vector<int>& values,
                                 std::size_t window) {
  if (window == 0 || window > values.size()) {
    throw std::invalid_argument{"invalid sliding window"};
  }
  std::deque<std::size_t> candidates;
  std::vector<int> result;
  for (std::size_t index = 0; index < values.size(); ++index) {
    while (!candidates.empty() && candidates.front() + window <= index) {
      candidates.pop_front();
    }
    while (!candidates.empty() &&
           values[candidates.back()] <= values[index]) {
      candidates.pop_back();
    }
    candidates.push_back(index);
    if (index + 1 >= window) {
      result.push_back(values[candidates.front()]);
    }
  }
  return result;
}

struct StreamingMoments final {
  std::size_t count{};
  double mean{};
  double m2{};

  void add(double value) {
    ++count;
    const double delta = value - mean;
    mean += delta / static_cast<double>(count);
    const double next_delta = value - mean;
    m2 += delta * next_delta;
  }

  [[nodiscard]] double sample_variance() const {
    if (count < 2) {
      throw std::logic_error{"sample variance requires two values"};
    }
    return m2 / static_cast<double>(count - 1);
  }
};

double maximum_drawdown(const std::vector<double>& equity) {
  if (equity.empty() || equity.front() <= 0.0) {
    throw std::invalid_argument{"equity curve must start positive"};
  }
  double peak = equity.front();
  double maximum = 0.0;
  for (const double value : equity) {
    if (value <= 0.0) {
      throw std::invalid_argument{"equity values must be positive"};
    }
    peak = std::max(peak, value);
    maximum = std::max(maximum, (peak - value) / peak);
  }
  return maximum;
}

std::vector<int> merge_sorted(const std::vector<std::vector<int>>& streams) {
  struct Cursor final {
    int value;
    std::size_t stream;
    std::size_t index;
  };
  const auto later = [](const Cursor& left, const Cursor& right) {
    return left.value > right.value;
  };
  std::priority_queue<Cursor, std::vector<Cursor>, decltype(later)> heap{later};
  for (std::size_t stream = 0; stream < streams.size(); ++stream) {
    if (!streams[stream].empty()) {
      heap.push(Cursor{streams[stream][0], stream, 0});
    }
  }
  std::vector<int> result;
  while (!heap.empty()) {
    const Cursor cursor = heap.top();
    heap.pop();
    result.push_back(cursor.value);
    const std::size_t next = cursor.index + 1;
    if (next < streams[cursor.stream].size()) {
      heap.push(Cursor{streams[cursor.stream][next], cursor.stream, next});
    }
  }
  return result;
}

int main() {
  const auto top = top_k({5, 1, 9, 3, 7}, 2);
  const auto lower = lower_bound_index({1, 3, 3, 7, 9}, 7);
  LruCache cache{2};
  cache.put(1, 10);
  cache.put(2, 20);
  const auto hit = cache.get(1);
  cache.put(3, 30);
  const auto maxima = sliding_maximum({2, 1, 5, 3, 8, 4}, 3);
  StreamingMoments moments;
  for (const double value : {1.0, 2.0, 3.0, 4.0, 5.0}) {
    moments.add(value);
  }
  const double drawdown = maximum_drawdown({100.0, 80.0, 120.0, 90.0});
  const auto merged = merge_sorted({{1, 4}, {2, 5}, {3, 6}});

  const bool valid = top == std::vector<int>{9, 7} && lower == 3 &&
                     hit == 10 && !cache.get(2).has_value() &&
                     maxima == std::vector<int>({5, 5, 8, 8}) &&
                     std::fabs(moments.mean - 3.0) < 1e-12 &&
                     std::fabs(moments.sample_variance() - 2.5) < 1e-12 &&
                     std::fabs(drawdown - 0.25) < 1e-12 &&
                     merged == std::vector<int>({1, 2, 3, 4, 5, 6});
  if (!valid) {
    std::cerr << "algorithm oracle mismatch\n";
    return 2;
  }
  std::cout << "algorithms-ok top=" << top.front()
            << " lower-bound=" << lower << " lru-hit=" << hit.value()
            << " window-max=" << maxima.back() << " mean="
            << std::fixed << std::setprecision(2) << moments.mean
            << " drawdown=" << drawdown << " merged=" << merged.size()
            << '\n';
}
