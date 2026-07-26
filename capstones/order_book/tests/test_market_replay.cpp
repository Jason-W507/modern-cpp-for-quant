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
  const auto rejects_as = [&replay](std::vector<std::uint8_t> bytes,
                                    quant::capstone::DecodeError expected) {
    const auto outcome = replay.apply(bytes);
    return outcome.status == quant::capstone::ReplayStatus::decode_error &&
           outcome.decode_error == expected && replay.next_expected() == 1;
  };
  bool decode_reasons_are_explicit = rejects_as(
      {1, 1, 0}, quant::capstone::DecodeError::frame_size);
  auto invalid_version = add_message(1, 10, 1, 100, 1);
  invalid_version[0] = 2;
  decode_reasons_are_explicit &= rejects_as(
      invalid_version, quant::capstone::DecodeError::version);
  auto invalid_type = add_message(1, 11, 1, 100, 1);
  invalid_type[1] = 2;
  decode_reasons_are_explicit &= rejects_as(
      invalid_type, quant::capstone::DecodeError::message_type);
  auto invalid_length = add_message(1, 12, 1, 100, 1);
  invalid_length[3] = 31;
  decode_reasons_are_explicit &= rejects_as(
      invalid_length, quant::capstone::DecodeError::declared_length);
  decode_reasons_are_explicit &= rejects_as(
      add_message(1, 13, 9, 100, 1), quant::capstone::DecodeError::side);
  auto invalid_reserved = add_message(1, 14, 1, 100, 1);
  invalid_reserved[21] = 1;
  decode_reasons_are_explicit &= rejects_as(
      invalid_reserved, quant::capstone::DecodeError::reserved_bytes);
  replay.apply(add_message(1, 1, 2, 101, 6));
  replay.apply(add_message(2, 2, 1, 100, 5));
  replay.apply(add_message(2, 99, 1, 999, 1));
  replay.apply(add_message(4, 4, 1, 102, 1));
  const auto outcome = replay.apply(add_message(3, 3, 1, 102, 4));

  const auto& stats = replay.stats();
  const auto& book = replay.book();
  const bool valid =
      outcome.status == quant::capstone::ReplayStatus::accepted &&
      outcome.trades.size() == 2 && stats.sequence_accepted == 4 &&
      stats.book_accepted == 4 && stats.book_rejected == 0 &&
      stats.duplicates == 1 && stats.gaps == 1 &&
      stats.buffered == 1 && stats.recovered == 1 &&
      stats.decode_errors == 6 && replay.next_expected() == 5 &&
      book.best_bid() == 100 && book.quantity_at(quant::capstone::Side::buy, 100) == 5 &&
      book.best_ask() == 101 && book.quantity_at(quant::capstone::Side::sell, 101) == 1;

  quant::capstone::MarketReplay bounded{quant::capstone::ReplayConfig{
      .max_sequence_gap = 3,
      .max_pending_messages = 2}};
  const auto too_far = bounded.apply(add_message(5, 50, 1, 100, 1));
  const auto first_buffered = bounded.apply(add_message(3, 30, 1, 100, 1));
  const auto second_buffered = bounded.apply(add_message(4, 40, 1, 100, 1));
  const auto full = bounded.apply(add_message(2, 20, 1, 100, 1));
  const bool bounds_hold =
      too_far.status == quant::capstone::ReplayStatus::sequence_gap_exceeded &&
      first_buffered.status == quant::capstone::ReplayStatus::gap &&
      second_buffered.status == quant::capstone::ReplayStatus::gap &&
      full.status == quant::capstone::ReplayStatus::pending_overflow &&
      bounded.pending_messages() == 2 && bounded.stats().gap_rejections == 1 &&
      bounded.stats().pending_overflows == 1 && bounded.next_expected() == 1;

  quant::capstone::MarketReplay recovered_rejection;
  recovered_rejection.apply(add_message(3, 0, 1, 100, 1));
  recovered_rejection.apply(add_message(1, 1, 1, 99, 1));
  const auto mixed =
      recovered_rejection.apply(add_message(2, 2, 1, 100, 1));
  const bool outcome_is_explicit =
      mixed.status == quant::capstone::ReplayStatus::accepted &&
      mixed.sequence_accepted == 2 && mixed.book_accepted == 1 &&
      mixed.book_rejected == 1 && mixed.recovered == 1 &&
      recovered_rejection.stats().sequence_accepted == 3 &&
      recovered_rejection.stats().book_accepted == 2 &&
      recovered_rejection.stats().book_rejected == 1;

  bool invalid_config_rejected = false;
  try {
    static_cast<void>(quant::capstone::MarketReplay{
        quant::capstone::ReplayConfig{.max_sequence_gap = 0,
                                     .max_pending_messages = 1}});
  } catch (const std::invalid_argument&) {
    invalid_config_rejected = true;
  }
  if (!valid || !bounds_hold || !outcome_is_explicit ||
      !decode_reasons_are_explicit ||
      !invalid_config_rejected) {
    std::cerr << "market replay oracle mismatch\n";
    return 2;
  }
  std::cout << "market-replay-tests-ok sequence-accepted=4 book-accepted=4 "
               "duplicate=1 gap=1 buffered=1 recovered=1 decode=6 trades=2 "
               "bounded=1 explicit-outcome=1 next=5\n";
}
