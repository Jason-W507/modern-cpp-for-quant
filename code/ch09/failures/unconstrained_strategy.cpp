struct Event {};

template <typename Strategy>
void decide(const Strategy& strategy, const Event& event) {
  strategy.on_event(event);
}

int main() {
  const Event event;
  decide(42, event);
}
