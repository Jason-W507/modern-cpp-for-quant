#pragma once

#include "quant/capstone/order_book.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <span>
#include <vector>

namespace quant::capstone {

enum class ReplayOverflowPolicy { reject_newest };

struct ReplayConfig final {
  std::uint64_t max_sequence_gap{1'024};
  std::size_t max_pending_messages{1'024};
  ReplayOverflowPolicy overflow_policy{ReplayOverflowPolicy::reject_newest};
};

struct ReplayStats final {
  std::size_t sequence_accepted{};
  std::size_t book_accepted{};
  std::size_t book_rejected{};
  std::size_t duplicates{};
  std::size_t gaps{};
  std::size_t gap_rejections{};
  std::size_t pending_overflows{};
  std::size_t buffered{};
  std::size_t recovered{};
  std::size_t decode_errors{};
  std::size_t trades{};
};

struct ReplayOutcome final {
  ReplayStatus status{};
  std::size_t sequence_accepted{};
  std::size_t book_accepted{};
  std::size_t book_rejected{};
  std::size_t recovered{};
  std::vector<Trade> trades;
};

class MarketReplay final {
 public:
  MarketReplay() = default;
  explicit MarketReplay(ReplayConfig config);

  ReplayOutcome apply(std::span<const std::uint8_t> bytes);

  [[nodiscard]] const ReplayStats& stats() const { return stats_; }
  [[nodiscard]] const LimitOrderBook& book() const { return book_; }
  [[nodiscard]] std::uint64_t next_expected() const {
    return sequence_.next_expected();
  }
  [[nodiscard]] std::size_t pending_messages() const {
    return pending_.size();
  }

 private:
  ReplayConfig config_;
  LimitOrderBook book_;
  SequenceGate sequence_;
  ReplayStats stats_;
  std::map<std::uint64_t, std::vector<std::uint8_t>> pending_;
};

}  // namespace quant::capstone
