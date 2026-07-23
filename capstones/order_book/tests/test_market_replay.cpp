#include "quant/capstone/market_replay.hpp"

#include <cstdint>
#include <iostream>
#include <span>
#include <vector>

namespace {

void append_u16(std::vector<std::uint8_t>& bytes, std::uint16_t value) {
  bytes.push_back(static_cast<std::uint8_t>(value >> 8));
  bytes.push_back(static_cast<std::uint8_t>(value));
}

void append_u32(std::vector<std::uint8_t>& bytes, std::uint32_t value) {
  for (int shift = 24; shift >= 0; shift -= 8) {
    bytes.push_back(static_cast<std::uint8_t>(value >> shift));
  }
}

void append_u64(std::vector<std::uint8_t>& bytes, std::uint64_t value) {
  for (int shift = 56; shift >= 0; shift -= 8) {
    bytes.push_back(static_cast<std::uint8_t>(value >> shift));
  }
}

std::vector<std::uint8_t> add_message(std::uint64_t sequence,
                                      std::uint64_t order_id,
                                      std::uint8_t side,
                                      std::int32_t price_ticks,
                                      std::int32_t quantity) {
  std::vector<std::uint8_t> bytes;
  bytes.reserve(32);
  bytes.push_back(1);  // protocol version
  bytes.push_back(1);  // add-order message
  append_u16(bytes, 32);
  append_u64(bytes, sequence);
  append_u64(bytes, order_id);
  bytes.push_back(side);
  bytes.insert(bytes.end(), 3, 0);
  append_u32(bytes, static_cast<std::uint32_t>(price_ticks));
  append_u32(bytes, static_cast<std::uint32_t>(quantity));
  return bytes;
}

}  // namespace

int main() {
  quant::capstone::MarketReplay replay;
  const std::vector<std::uint8_t> malformed{1, 1, 0};
  replay.apply(malformed);
  replay.apply(add_message(1, 1, 2, 101, 6));
  replay.apply(add_message(2, 2, 1, 100, 5));
  replay.apply(add_message(2, 99, 1, 999, 1));
  replay.apply(add_message(4, 4, 1, 102, 1));
  const auto outcome = replay.apply(add_message(3, 3, 1, 102, 4));

  const auto& stats = replay.stats();
  const auto& book = replay.book();
  const bool valid =
      outcome.status == quant::capstone::ReplayStatus::accepted &&
      outcome.trades.size() == 2 && stats.accepted == 4 &&
      stats.duplicates == 1 && stats.gaps == 1 &&
      stats.buffered == 1 && stats.recovered == 1 &&
      stats.decode_errors == 1 && replay.next_expected() == 5 &&
      book.best_bid() == 100 && book.quantity_at(quant::capstone::Side::buy, 100) == 5 &&
      book.best_ask() == 101 && book.quantity_at(quant::capstone::Side::sell, 101) == 1;
  if (!valid) {
    std::cerr << "market replay oracle mismatch\n";
    return 2;
  }
  std::cout << "market-replay-tests-ok accepted=4 duplicate=1 gap=1 buffered=1 "
               "recovered=1 decode=1 trades=2 next=5\n";
}
