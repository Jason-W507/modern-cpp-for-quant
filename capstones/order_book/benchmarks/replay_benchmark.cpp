#include "quant/capstone/market_replay.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

void append(std::vector<std::uint8_t>& bytes, std::uint64_t value,
            std::size_t width) {
  for (std::size_t index = 0; index < width; ++index) {
    const auto shift = static_cast<unsigned>((width - index - 1) * 8);
    bytes.push_back(static_cast<std::uint8_t>(value >> shift));
  }
}

std::vector<std::uint8_t> add_message(std::uint64_t sequence) {
  std::vector<std::uint8_t> bytes;
  bytes.reserve(32);
  bytes.push_back(1);
  bytes.push_back(1);
  append(bytes, 32, 2);
  append(bytes, sequence, 8);
  append(bytes, sequence, 8);
  bytes.push_back(1);
  bytes.insert(bytes.end(), 3, 0);
  append(bytes, 100, 4);
  append(bytes, 1, 4);
  return bytes;
}

std::int64_t percentile(const std::vector<std::int64_t>& sorted,
                        double probability) {
  const auto index = static_cast<std::size_t>(
      probability * static_cast<double>(sorted.size() - 1));
  return sorted[index];
}

}  // namespace

int main() {
  constexpr std::size_t message_count = 10'000;
  std::vector<std::vector<std::uint8_t>> messages;
  messages.reserve(message_count);
  for (std::size_t index = 0; index < message_count; ++index) {
    messages.push_back(add_message(index + 1));
  }

  quant::capstone::MarketReplay replay;
  std::vector<std::int64_t> samples;
  samples.reserve(message_count);
  const auto run_start = std::chrono::steady_clock::now();
  for (const auto& message : messages) {
    const auto start = std::chrono::steady_clock::now();
    replay.apply(message);
    const auto stop = std::chrono::steady_clock::now();
    samples.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start)
            .count());
  }
  const auto run_stop = std::chrono::steady_clock::now();
  std::sort(samples.begin(), samples.end());
  const double seconds = std::chrono::duration<double>(run_stop - run_start).count();

  if (replay.stats().accepted != message_count ||
      replay.next_expected() != message_count + 1) {
    std::cerr << "replay benchmark correctness gate failed\n";
    return 2;
  }
  std::cout << "replay-benchmark-ok messages=" << message_count
            << " accepted=" << replay.stats().accepted
            << " p50_ns=" << percentile(samples, 0.50)
            << " p99_ns=" << percentile(samples, 0.99)
            << " p999_ns=" << percentile(samples, 0.999)
            << " throughput_msg_s=" << std::fixed << std::setprecision(0)
            << static_cast<double>(message_count) / seconds << '\n';
}
