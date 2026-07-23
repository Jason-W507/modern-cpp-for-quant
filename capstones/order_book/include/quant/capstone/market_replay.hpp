#pragma once

#include "quant/capstone/order_book.hpp"

#include <cstddef>
#include <cstdint>
#include <map>
#include <span>
#include <vector>

namespace quant::capstone {

struct ReplayStats final {
  std::size_t accepted{};
  std::size_t duplicates{};
  std::size_t gaps{};
  std::size_t buffered{};
  std::size_t recovered{};
  std::size_t decode_errors{};
  std::size_t book_rejections{};
  std::size_t trades{};
};

struct ReplayOutcome final {
  ReplayStatus status{};
  std::vector<Trade> trades;
};

class MarketReplay final {
 public:
  ReplayOutcome apply(std::span<const std::uint8_t> bytes);

  [[nodiscard]] const ReplayStats& stats() const { return stats_; }
  [[nodiscard]] const LimitOrderBook& book() const { return book_; }
  [[nodiscard]] std::uint64_t next_expected() const {
    return sequence_.next_expected();
  }

 private:
  LimitOrderBook book_;
  SequenceGate sequence_;
  ReplayStats stats_;
  std::map<std::uint64_t, std::vector<std::uint8_t>> pending_;
};

}  // namespace quant::capstone
