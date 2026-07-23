#include "quant/capstone/market_replay.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>

namespace quant::capstone {
namespace {

constexpr std::uint8_t protocol_version = 1;
constexpr std::uint8_t add_message_type = 1;
constexpr std::uint8_t buy_side_code = 1;
constexpr std::uint8_t sell_side_code = 2;
constexpr std::size_t version_offset = 0;
constexpr std::size_t type_offset = 1;
constexpr std::size_t length_offset = 2;
constexpr std::size_t sequence_offset = 4;
constexpr std::size_t order_id_offset = 12;
constexpr std::size_t side_offset = 20;
constexpr std::size_t reserved_begin = 21;
constexpr std::size_t price_offset = 24;
constexpr std::size_t quantity_offset = 28;
constexpr std::size_t add_message_size = 32;

std::uint16_t read_u16(std::span<const std::uint8_t> bytes,
                       std::size_t offset) {
  return static_cast<std::uint16_t>(
      (static_cast<std::uint16_t>(bytes[offset]) << 8U) |
      static_cast<std::uint16_t>(bytes[offset + 1]));
}

std::uint32_t read_u32(std::span<const std::uint8_t> bytes,
                       std::size_t offset) {
  std::uint32_t value{};
  for (std::size_t index = 0; index < 4; ++index) {
    value = (value << 8U) | bytes[offset + index];
  }
  return value;
}

std::uint64_t read_u64(std::span<const std::uint8_t> bytes,
                       std::size_t offset) {
  std::uint64_t value{};
  for (std::size_t index = 0; index < 8; ++index) {
    value = (value << 8U) | bytes[offset + index];
  }
  return value;
}

struct AddMessage final {
  std::uint64_t sequence{};
  Order order;
};

std::optional<AddMessage> decode_add(
    std::span<const std::uint8_t> bytes) {
  if (bytes.size() != add_message_size ||
      bytes[version_offset] != protocol_version ||
      bytes[type_offset] != add_message_type ||
      read_u16(bytes, length_offset) != add_message_size) {
    return std::nullopt;
  }
  const std::uint8_t side_code = bytes[side_offset];
  if (side_code != buy_side_code && side_code != sell_side_code) {
    return std::nullopt;
  }
  for (std::size_t offset = reserved_begin; offset < price_offset; ++offset) {
    if (bytes[offset] != 0) {
      return std::nullopt;
    }
  }
  return AddMessage{
      read_u64(bytes, sequence_offset),
      Order{read_u64(bytes, order_id_offset),
            side_code == buy_side_code ? Side::buy : Side::sell,
            std::bit_cast<std::int32_t>(read_u32(bytes, price_offset)),
            std::bit_cast<std::int32_t>(read_u32(bytes, quantity_offset))}};
}

}  // namespace

ReplayOutcome MarketReplay::apply(std::span<const std::uint8_t> bytes) {
  const auto message = decode_add(bytes);
  if (!message) {
    ++stats_.decode_errors;
    return {ReplayStatus::decode_error, {}};
  }

  const ReplayStatus sequence_status = sequence_.observe(message->sequence);
  if (sequence_status == ReplayStatus::duplicate_or_stale) {
    ++stats_.duplicates;
    return {sequence_status, {}};
  }
  if (sequence_status == ReplayStatus::gap) {
    ++stats_.gaps;
    return {sequence_status, {}};
  }

  try {
    auto trades = book_.submit(message->order);
    ++stats_.accepted;
    stats_.trades += trades.size();
    return {ReplayStatus::accepted, std::move(trades)};
  } catch (const std::invalid_argument&) {
    ++stats_.book_rejections;
    return {ReplayStatus::book_rejected, {}};
  }
}

}  // namespace quant::capstone
