#include "quant/capstone/market_replay.hpp"
#include "quant/capstone/spsc_ring.hpp"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>
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

std::vector<std::uint8_t> add_message(std::uint64_t sequence) {
  std::vector<std::uint8_t> bytes;
  bytes.reserve(32);
  bytes.push_back(1);
  bytes.push_back(1);
  append_u16(bytes, 32);
  append_u64(bytes, sequence);
  append_u64(bytes, sequence);
  bytes.push_back(2);
  bytes.insert(bytes.end(), 3, 0);
  append_u32(bytes, 101);
  append_u32(bytes, 1);
  return bytes;
}

}  // namespace

int main() {
  constexpr std::uint64_t message_count = 10'000;
  quant::capstone::SpscRing<std::vector<std::uint8_t>, 256> queue;
  quant::capstone::MarketReplay replay;
  std::atomic<bool> producer_done{false};

  std::jthread producer{[&] {
    for (std::uint64_t sequence = 1; sequence <= message_count; ++sequence) {
      auto message = add_message(sequence);
      while (!queue.try_push(std::move(message))) {
        std::this_thread::yield();
      }
    }
    producer_done.store(true, std::memory_order_release);
  }};

  std::jthread consumer{[&] {
    while (!producer_done.load(std::memory_order_acquire) || !queue.empty()) {
      if (auto message = queue.try_pop()) {
        replay.apply(*message);
      } else {
        std::this_thread::yield();
      }
    }
  }};
  producer.join();
  consumer.join();

  if (replay.stats().sequence_accepted != message_count ||
      replay.stats().book_accepted != message_count ||
      replay.next_expected() != message_count + 1) {
    std::cerr << "SPSC replay oracle mismatch\n";
    return 2;
  }
  std::cout << "spsc-replay-ok offered=10000 sequence-accepted=10000 "
               "book-accepted=10000 next=10001\n";
}
