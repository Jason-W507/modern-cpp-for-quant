#include <iomanip>
#include <iostream>

int main() {
  constexpr double baseline_total_ms = 100.0;
  constexpr double hotspot_ms = 60.0;
  constexpr double local_speedup = 4.0;

  constexpr double unaffected_ms = baseline_total_ms - hotspot_ms;
  constexpr double predicted_total_ms =
      unaffected_ms + hotspot_ms / local_speedup;
  constexpr double overall_speedup =
      baseline_total_ms / predicted_total_ms;
  constexpr double ceiling_speedup = baseline_total_ms / unaffected_ms;

  std::cout << std::fixed << std::setprecision(2)
            << "amdahl-ok total-ms=" << baseline_total_ms
            << " hotspot-ms=" << hotspot_ms
            << " fraction=" << hotspot_ms / baseline_total_ms
            << " local-speedup=" << local_speedup
            << " predicted-total-ms=" << predicted_total_ms
            << " overall-speedup=" << overall_speedup
            << " ceiling-speedup=" << ceiling_speedup << '\n';
}
