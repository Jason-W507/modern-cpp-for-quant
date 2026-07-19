#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>

constexpr std::size_t experiment_line_bytes = 64;

struct alignas(experiment_line_bytes) CounterSlot final {
  std::atomic<std::uint64_t> value{};
};

int main() {
  std::array<CounterSlot, 2> slots{};
  const auto first = reinterpret_cast<std::uintptr_t>(&slots[0]);
  const auto second = reinterpret_cast<std::uintptr_t>(&slots[1]);
  const std::size_t stride = second - first;
  const bool separated_by_assumption = stride >= experiment_line_bytes;
  if (!separated_by_assumption ||
      alignof(CounterSlot) < experiment_line_bytes) {
    std::cerr << "counter slots violate the experiment assumption\n";
    return 1;
  }

  std::cout << "false-sharing-layout-ok assumed-line-bytes="
            << experiment_line_bytes << " slot-size=" << sizeof(CounterSlot)
            << " slot-stride=" << stride
            << " separated-by-assumption=" << std::boolalpha
            << separated_by_assumption << '\n';
}
