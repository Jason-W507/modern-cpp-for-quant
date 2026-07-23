#include <algorithm>
#include <cmath>
#include <cstddef>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <limits>
#include <optional>
#include <queue>
#include <set>
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

int rotated_minimum(const std::vector<int>& values) {
  if (values.empty()) {
    throw std::invalid_argument{"rotated array must not be empty"};
  }
  std::size_t first{};
  std::size_t last = values.size() - 1;
  while (first < last) {
    const std::size_t middle = first + (last - first) / 2;
    if (values[middle] < values[last]) {
      last = middle;
    } else if (values[middle] > values[last]) {
      first = middle + 1;
    } else {
      --last;
    }
  }
  return values[first];
}

using Interval = std::pair<int, int>;

std::vector<Interval> merge_intervals(std::vector<Interval> intervals) {
  std::ranges::sort(intervals);
  std::vector<Interval> result;
  for (const auto& [first, last] : intervals) {
    if (first > last) {
      throw std::invalid_argument{"interval starts after it ends"};
    }
    if (result.empty() || first > result.back().second) {
      result.push_back({first, last});
    } else {
      result.back().second = std::max(result.back().second, last);
    }
  }
  return result;
}

class FenwickTree final {
 public:
  explicit FenwickTree(std::size_t size) : tree_(size + 1) {}

  void add(std::size_t index, int delta) {
    for (++index; index < tree_.size(); index += index & (~index + 1)) {
      tree_[index] += delta;
    }
  }

  [[nodiscard]] int prefix_sum(std::size_t inclusive_index) const {
    int sum{};
    for (++inclusive_index; inclusive_index > 0;
         inclusive_index -= inclusive_index & (~inclusive_index + 1)) {
      sum += tree_[inclusive_index];
    }
    return sum;
  }

 private:
  std::vector<int> tree_;
};

class DisjointSet final {
 public:
  explicit DisjointSet(std::size_t size) : parent_(size), rank_(size) {
    for (std::size_t index = 0; index < size; ++index) {
      parent_[index] = index;
    }
  }

  std::size_t find(std::size_t value) {
    if (parent_[value] != value) {
      parent_[value] = find(parent_[value]);
    }
    return parent_[value];
  }

  void unite(std::size_t left, std::size_t right) {
    left = find(left);
    right = find(right);
    if (left == right) {
      return;
    }
    if (rank_[left] < rank_[right]) {
      std::swap(left, right);
    }
    parent_[right] = left;
    if (rank_[left] == rank_[right]) {
      ++rank_[left];
    }
  }

 private:
  std::vector<std::size_t> parent_;
  std::vector<unsigned> rank_;
};

class SlidingMedian final {
 public:
  void add(double value) {
    if (lower_.empty() || value <= *lower_.rbegin()) {
      lower_.insert(value);
    } else {
      upper_.insert(value);
    }
    rebalance();
  }

  void remove(double value) {
    if (const auto found = lower_.find(value); found != lower_.end()) {
      lower_.erase(found);
    } else if (const auto found = upper_.find(value); found != upper_.end()) {
      upper_.erase(found);
    } else {
      throw std::invalid_argument{"sliding median removed an absent value"};
    }
    rebalance();
  }

  [[nodiscard]] double value() const {
    if (lower_.empty()) {
      throw std::logic_error{"median requires at least one value"};
    }
    if (lower_.size() == upper_.size()) {
      return (*lower_.rbegin() + *upper_.begin()) / 2.0;
    }
    return *lower_.rbegin();
  }

 private:
  void rebalance() {
    while (lower_.size() > upper_.size() + 1) {
      const auto last = std::prev(lower_.end());
      upper_.insert(*last);
      lower_.erase(last);
    }
    while (upper_.size() > lower_.size()) {
      const auto first = upper_.begin();
      lower_.insert(*first);
      upper_.erase(first);
    }
  }

  std::multiset<double> lower_;
  std::multiset<double> upper_;
};

std::vector<double> sliding_medians(const std::vector<double>& values,
                                    std::size_t window) {
  if (window == 0 || window > values.size()) {
    throw std::invalid_argument{"invalid median window"};
  }
  SlidingMedian median;
  std::vector<double> result;
  for (std::size_t index = 0; index < values.size(); ++index) {
    median.add(values[index]);
    if (index >= window) {
      median.remove(values[index - window]);
    }
    if (index + 1 >= window) {
      result.push_back(median.value());
    }
  }
  return result;
}

class TtlCache final {
 public:
  void put(int key, int value, int expires_at) {
    entries_[key] = Entry{value, expires_at};
  }

  std::optional<int> get(int key, int now) {
    const auto found = entries_.find(key);
    if (found == entries_.end()) {
      return std::nullopt;
    }
    if (now >= found->second.expires_at) {
      entries_.erase(found);
      return std::nullopt;
    }
    return found->second.value;
  }

 private:
  struct Entry final {
    int value;
    int expires_at;
  };
  std::unordered_map<int, Entry> entries_;
};

class CancelableOrderQueue final {
 public:
  void add(int order_id) {
    if (index_.contains(order_id)) {
      throw std::invalid_argument{"duplicate order id"};
    }
    orders_.push_back(order_id);
    index_[order_id] = std::prev(orders_.end());
  }

  bool cancel(int order_id) {
    const auto found = index_.find(order_id);
    if (found == index_.end()) {
      return false;
    }
    orders_.erase(found->second);
    index_.erase(found);
    return true;
  }

  [[nodiscard]] std::vector<int> order_ids() const {
    return {orders_.begin(), orders_.end()};
  }

 private:
  std::list<int> orders_;
  std::unordered_map<int, std::list<int>::iterator> index_;
};

std::vector<std::size_t> topological_order(
    const std::vector<std::vector<std::size_t>>& edges) {
  std::vector<std::size_t> indegree(edges.size());
  for (const auto& outgoing : edges) {
    for (const std::size_t target : outgoing) {
      if (target >= edges.size()) {
        throw std::invalid_argument{"topology edge is out of range"};
      }
      ++indegree[target];
    }
  }
  std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<>> ready;
  for (std::size_t node = 0; node < edges.size(); ++node) {
    if (indegree[node] == 0) {
      ready.push(node);
    }
  }
  std::vector<std::size_t> order;
  while (!ready.empty()) {
    const std::size_t node = ready.top();
    ready.pop();
    order.push_back(node);
    for (const std::size_t target : edges[node]) {
      if (--indegree[target] == 0) {
        ready.push(target);
      }
    }
  }
  if (order.size() != edges.size()) {
    throw std::invalid_argument{"topology contains a cycle"};
  }
  return order;
}

struct WeightedEdge final {
  std::size_t target;
  int cost;
};

int shortest_cost(const std::vector<std::vector<WeightedEdge>>& graph,
                  std::size_t source, std::size_t target) {
  if (source >= graph.size() || target >= graph.size()) {
    throw std::invalid_argument{"shortest-path endpoint is out of range"};
  }
  constexpr int infinity = std::numeric_limits<int>::max();
  std::vector<int> distance(graph.size(), infinity);
  using QueueEntry = std::pair<int, std::size_t>;
  std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<>> ready;
  distance[source] = 0;
  ready.push({0, source});
  while (!ready.empty()) {
    const auto [cost, node] = ready.top();
    ready.pop();
    if (cost != distance[node]) {
      continue;
    }
    for (const auto [next, edge_cost] : graph[node]) {
      if (next >= graph.size() || edge_cost < 0) {
        throw std::invalid_argument{"Dijkstra requires valid nonnegative edges"};
      }
      if (cost <= infinity - edge_cost && cost + edge_cost < distance[next]) {
        distance[next] = cost + edge_cost;
        ready.push({distance[next], next});
      }
    }
  }
  return distance[target];
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
  FenwickTree fenwick{4};
  fenwick.add(1, 5);
  fenwick.add(3, 2);
  DisjointSet groups{4};
  groups.unite(0, 1);
  groups.unite(1, 2);
  const auto topology = topological_order({{1, 2}, {3}, {3}, {}});
  const auto shortest = shortest_cost(
      {{{1, 3}, {2, 10}}, {{2, 3}}, {}}, 0, 2);
  const auto medians = sliding_medians({5, 1, 4, 2, 3, 8}, 5);
  TtlCache ttl;
  ttl.put(1, 42, 15);
  const auto ttl_hit = ttl.get(1, 14);
  const auto ttl_miss = ttl.get(1, 15);
  CancelableOrderQueue orders;
  orders.add(10);
  orders.add(11);
  orders.add(12);
  const bool cancelled = orders.cancel(11);

  const bool valid = top == std::vector<int>{9, 7} && lower == 3 &&
                     hit == 10 && !cache.get(2).has_value() &&
                     maxima == std::vector<int>({5, 5, 8, 8}) &&
                     std::fabs(moments.mean - 3.0) < 1e-12 &&
                     std::fabs(moments.sample_variance() - 2.5) < 1e-12 &&
                     std::fabs(drawdown - 0.25) < 1e-12 &&
                     merged == std::vector<int>({1, 2, 3, 4, 5, 6}) &&
                     rotated_minimum({2, 2, 2, 0, 1}) == 0 &&
                     merge_intervals({{9, 11}, {10, 12}, {13, 14}}) ==
                         std::vector<Interval>({{9, 12}, {13, 14}}) &&
                     fenwick.prefix_sum(2) == 5 &&
                     fenwick.prefix_sum(3) == 7 && groups.find(0) == groups.find(2) &&
                     groups.find(0) != groups.find(3) &&
                     topology == std::vector<std::size_t>({0, 1, 2, 3}) &&
                     shortest == 6 &&
                     medians == std::vector<double>({3.0, 3.0}) &&
                     ttl_hit == 42 && !ttl_miss.has_value() && cancelled &&
                     orders.order_ids() == std::vector<int>({10, 12});
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
