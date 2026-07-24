#pragma once

#include <algorithm>
#include <cstdint>
#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <vector>

namespace quant::capstone {

enum class Side { buy, sell };

struct Order final {
  std::uint64_t id{};
  Side side{};
  std::int64_t price_ticks{};
  std::int64_t quantity{};
};

struct Trade final {
  std::uint64_t maker_id{};
  std::uint64_t taker_id{};
  std::int64_t price_ticks{};
  std::int64_t quantity{};
};

class LimitOrderBook final {
 public:
  std::vector<Trade> submit(Order incoming) {
    validate(incoming);
    if (!seen_ids_.insert(incoming.id).second) {
      throw std::invalid_argument{"duplicate order id"};
    }

    std::vector<Trade> trades;
    if (incoming.side == Side::buy) {
      match(incoming, asks_,
            [](std::int64_t incoming_price, std::int64_t resting_price) {
              return incoming_price >= resting_price;
            },
            trades);
      if (incoming.quantity > 0) {
        bids_[incoming.price_ticks].push_back(incoming);
      }
    } else {
      match(incoming, bids_,
            [](std::int64_t incoming_price, std::int64_t resting_price) {
              return incoming_price <= resting_price;
            },
            trades);
      if (incoming.quantity > 0) {
        asks_[incoming.price_ticks].push_back(incoming);
      }
    }
    return trades;
  }

  [[nodiscard]] std::optional<std::int64_t> best_bid() const {
    return bids_.empty() ? std::nullopt
                         : std::optional<std::int64_t>{bids_.begin()->first};
  }

  [[nodiscard]] std::optional<std::int64_t> best_ask() const {
    return asks_.empty() ? std::nullopt
                         : std::optional<std::int64_t>{asks_.begin()->first};
  }

  [[nodiscard]] std::int64_t quantity_at(Side side,
                                         std::int64_t price_ticks) const {
    if (side == Side::sell) {
      return ask_quantity_at(price_ticks);
    }
    const auto found = bids_.find(price_ticks);
    return found == bids_.end() ? 0 : sum_level(found->second);
  }

 private:
  using BidLevels =
      std::map<std::int64_t, std::deque<Order>, std::greater<>>;
  using AskLevels = std::map<std::int64_t, std::deque<Order>>;

  static void validate(const Order& order) {
    if (order.id == 0 || order.price_ticks <= 0 || order.quantity <= 0) {
      throw std::invalid_argument{"order fields must be positive"};
    }
  }

  template <typename Levels, typename Crosses>
  static void match(Order& incoming, Levels& opposite, Crosses crosses,
                    std::vector<Trade>& trades) {
    while (incoming.quantity > 0 && !opposite.empty() &&
           crosses(incoming.price_ticks, opposite.begin()->first)) {
      auto level = opposite.begin();
      auto& orders = level->second;
      Order& resting = orders.front();
      const std::int64_t quantity =
          std::min(incoming.quantity, resting.quantity);
      trades.push_back(
          Trade{resting.id, incoming.id, resting.price_ticks, quantity});
      incoming.quantity -= quantity;
      resting.quantity -= quantity;
      if (resting.quantity == 0) {
        orders.pop_front();
      }
      if (orders.empty()) {
        opposite.erase(level);
      }
    }
  }

  [[nodiscard]] std::int64_t ask_quantity_at(std::int64_t price_ticks) const {
    const auto found = asks_.find(price_ticks);
    return found == asks_.end() ? 0 : sum_level(found->second);
  }

  static std::int64_t sum_level(const std::deque<Order>& orders) {
    std::int64_t result = 0;
    for (const auto& order : orders) {
      result += order.quantity;
    }
    return result;
  }

  BidLevels bids_;
  AskLevels asks_;
  std::unordered_set<std::uint64_t> seen_ids_;
};

enum class ReplayStatus {
  accepted,
  duplicate_or_stale,
  gap,
  sequence_gap_exceeded,
  pending_overflow,
  decode_error,
  book_rejected
};

class SequenceGate final {
 public:
  [[nodiscard]] ReplayStatus observe(std::uint64_t sequence) {
    if (sequence < next_) {
      return ReplayStatus::duplicate_or_stale;
    }
    if (sequence > next_) {
      return ReplayStatus::gap;
    }
    ++next_;
    return ReplayStatus::accepted;
  }

  [[nodiscard]] std::uint64_t next_expected() const { return next_; }

 private:
  std::uint64_t next_{1};
};

}  // namespace quant::capstone
