#include <cstddef>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

class TrackedQuote final {
 public:
  explicit TrackedQuote(double price) : price_{price} {}
  TrackedQuote(const TrackedQuote&) = delete;
  TrackedQuote& operator=(const TrackedQuote&) = delete;
  TrackedQuote(TrackedQuote&& other) noexcept : price_{other.price_} {
    ++moves_;
  }
  TrackedQuote& operator=(TrackedQuote&&) = delete;

  [[nodiscard]] double price() const { return price_; }
  static void reset_moves() { moves_ = 0; }
  [[nodiscard]] static std::size_t moves() { return moves_; }

 private:
  double price_{};
  static inline std::size_t moves_{};
};

struct BuildResult final {
  double checksum;
  std::size_t moves;
};

BuildResult build_quotes(bool reserve_first) {
  TrackedQuote::reset_moves();
  std::vector<TrackedQuote> quotes;
  if (reserve_first) {
    quotes.reserve(32);
  }
  for (int index = 0; index < 32; ++index) {
    quotes.emplace_back(100.0 + static_cast<double>(index));
  }

  double checksum = 0.0;
  for (const TrackedQuote& quote : quotes) {
    checksum += quote.price();
  }
  return BuildResult{checksum, TrackedQuote::moves()};
}

int main() {
  const BuildResult unreserved = build_quotes(false);
  const BuildResult reserved = build_quotes(true);
  if (unreserved.checksum != 3'696.0 ||
      reserved.checksum != unreserved.checksum || reserved.moves != 0) {
    std::cerr << "preallocation changed results or moved reserved elements\n";
    return 1;
  }

  std::cout << std::fixed << std::setprecision(2)
            << "preallocation-ok rows=32 checksum=" << reserved.checksum
            << " unreserved-moves=" << unreserved.moves
            << " reserved-moves=" << reserved.moves << '\n';
}
