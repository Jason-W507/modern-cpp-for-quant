#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <vector>

#include "quant/backtest.hpp"

int main() {
  constexpr std::size_t event_count = 200'000;
  constexpr std::size_t warmup_count = 2;
  constexpr std::size_t sample_count = 11;

  std::vector<quant::MarketEvent> events;
  events.reserve(event_count);
  events.push_back({{}, "AAPL", 99.0, 1'000});
  for (std::size_t index = 1; index < event_count; ++index) {
    events.push_back({{}, "AAPL", 103.0, 1'000});
  }

  const quant::ThresholdStrategy strategy{100.0, 25};
  const quant::BacktestEngine engine{10'000.0};
  auto run_once = [&] { return engine.run(events, strategy); };

  bool ledger_valid = true;
  double checksum = 0.0;
  for (std::size_t index = 0; index < warmup_count; ++index) {
    const auto result = run_once();
    ledger_valid = ledger_valid && result.fills.size() == 1 &&
                   result.final_portfolio.equity == 10'100.0;
    checksum += result.final_portfolio.equity;
  }

  std::vector<long long> samples;
  samples.reserve(sample_count);
  for (std::size_t index = 0; index < sample_count; ++index) {
    const auto start = std::chrono::steady_clock::now();
    const auto result = run_once();
    const auto elapsed = std::chrono::steady_clock::now() - start;
    samples.push_back(
        std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
    ledger_valid = ledger_valid && result.fills.size() == 1 &&
                   result.final_portfolio.equity == 10'100.0;
    checksum += result.final_portfolio.equity;
  }

  auto sorted_samples = samples;
  std::ranges::sort(sorted_samples);
  const long long median_us = sorted_samples[sample_count / 2];
  const long long iqr_us =
      sorted_samples[(sample_count * 3) / 4] -
      sorted_samples[sample_count / 4];
  if (!ledger_valid) {
    std::cerr << "benchmark-invalid hand-ledger-mismatch\n";
    return 2;
  }

  std::cout << "benchmark-ok events=" << event_count
            << " samples=" << sample_count << " warmups=" << warmup_count
            << " equity=10100 fills=1 raw-us=";
  for (std::size_t index = 0; index < samples.size(); ++index) {
    std::cout << (index == 0 ? "" : ",") << samples[index];
  }
  std::cout << " median-us=" << median_us
            << " iqr-us=" << iqr_us << " checksum=" << checksum << '\n';
}
