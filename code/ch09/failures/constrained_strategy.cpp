#include <concepts>

struct Event {};

template <typename Strategy>
concept EventStrategy = requires(const Strategy& strategy, const Event& event) {
  { strategy.on_event(event) } -> std::same_as<int>;
};

template <EventStrategy Strategy>
void decide(const Strategy& strategy, const Event& event) {
  strategy.on_event(event);
}

int main() {
  const Event event;
  decide(42, event);
}
