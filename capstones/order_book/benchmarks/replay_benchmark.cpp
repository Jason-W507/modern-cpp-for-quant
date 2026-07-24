#include "quant/capstone/market_replay.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>
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
  const auto rank = static_cast<std::size_t>(
      std::ceil(probability * static_cast<double>(sorted.size())));
  const auto index = std::clamp(rank, std::size_t{1}, sorted.size()) - 1;
  return sorted[index];
}

void write_samples(std::ostream& output,
                   const std::vector<std::int64_t>& samples) {
  output << '[';
  for (std::size_t index = 0; index < samples.size(); ++index) {
    output << (index == 0 ? "" : ", ") << samples[index];
  }
  output << ']';
}

}  // namespace

int main(int argc, char** argv) {
  constexpr std::size_t message_count = 10'000;
  constexpr std::size_t window_size = 100;
  std::vector<std::vector<std::uint8_t>> messages;
  messages.reserve(message_count);
  for (std::size_t index = 0; index < message_count; ++index) {
    messages.push_back(add_message(index + 1));
  }

  quant::capstone::MarketReplay replay;
  std::vector<std::int64_t> samples;
  samples.reserve(message_count / window_size);
  const auto run_start = std::chrono::steady_clock::now();
  for (std::size_t first = 0; first < message_count; first += window_size) {
    const auto start = std::chrono::steady_clock::now();
    for (std::size_t index = first; index < first + window_size; ++index) {
      replay.apply(messages[index]);
    }
    const auto stop = std::chrono::steady_clock::now();
    samples.push_back(
        std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start)
            .count() /
        static_cast<std::int64_t>(window_size));
  }
  const auto run_stop = std::chrono::steady_clock::now();
  std::sort(samples.begin(), samples.end());
  const double seconds = std::chrono::duration<double>(run_stop - run_start).count();

  if (replay.stats().sequence_accepted != message_count ||
      replay.stats().book_accepted != message_count ||
      replay.next_expected() != message_count + 1) {
    std::cerr << "replay benchmark correctness gate failed\n";
    return 2;
  }
  const auto p50 = percentile(samples, 0.50);
  const auto p99 = percentile(samples, 0.99);
  const auto throughput = static_cast<double>(message_count) / seconds;
  if (argc == 3 && std::string_view{argv[1]} == "--json") {
    std::ofstream output{argv[2]};
    if (!output) {
      std::cerr << "cannot open replay benchmark report\n";
      return 3;
    }
    output << "{\n  \"schema\": 1,\n  \"environment\": {\"compiler\": \""
           << __VERSION__
           << "\", \"clock\": \"std::chrono::steady_clock\"},\n"
              "  \"workload\": {\"messages\": "
           << message_count
           << ", \"message_bytes\": 32, \"pre_encoded\": true, \"window_messages\": "
           << window_size
           << ", \"sample_unit\": \"ns_per_message_from_window\"},\n"
              "  \"samples_ns\": ";
    write_samples(output, samples);
    output << ",\n  \"summary\": {\"p50_ns\": " << p50
           << ", \"p99_ns\": " << p99
           << ", \"throughput_msg_s\": " << std::fixed
           << std::setprecision(0) << throughput
           << "},\n  \"correctness\": {\"sequence_accepted\": "
           << replay.stats().sequence_accepted << ", \"book_accepted\": "
           << replay.stats().book_accepted << ", \"next_expected\": "
           << replay.next_expected()
           << "},\n  \"limitations\": [\"one process; use run_replay_benchmark.py for multi-process evidence\", \"window averages do not expose within-window single-message tails\", \"no affinity or frequency pinning\", \"teaching protocol, not an exchange feed\"]\n}\n";
  }
  std::cout << "replay-benchmark-ok messages=" << message_count
            << " sequence-accepted=" << replay.stats().sequence_accepted
            << " book-accepted=" << replay.stats().book_accepted
            << " p50_ns=" << p50
            << " p99_ns=" << p99
            << " throughput_msg_s=" << std::fixed << std::setprecision(0)
            << throughput << '\n';
}
