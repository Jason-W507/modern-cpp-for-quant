#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "quant/ch08/execution.hpp"
#include "quant/ch08/event_reader.hpp"
#include "quant/ch08/strategy.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: ch08_domain_pipeline <events.csv>\n";
    return 2;
  }

  try {
    const auto events = quant::ch08::read_events(argv[1]);

    const quant::ch08::ThresholdStrategy threshold{100.0, 25};
    const quant::ch08::Strategy& strategy = threshold;
    const quant::ch08::SimulatedExchange exchange;
    quant::ch08::Portfolio portfolio{10'000.0};
    std::size_t order_count = 0;
    std::size_t fill_count = 0;

    for (const quant::ch08::MarketEvent& event : events) {
      const auto before = portfolio.snapshot(event.symbol(), event.price());
      const auto order = strategy.on_event(event, before);
      if (!order.has_value()) {
        continue;
      }
      ++order_count;
      const auto fill = exchange.match(*order, event);
      if (fill.has_value()) {
        portfolio.apply(*fill);
        ++fill_count;
      }
    }

    const quant::ch08::MarketEvent& last = events.back();
    const auto result = portfolio.snapshot(last.symbol(), last.price());
    std::cout << std::fixed << std::setprecision(2)
              << "orders=" << order_count << " fills=" << fill_count
              << " symbol=" << result.symbol
              << " quantity=" << result.quantity << " cash=" << result.cash
              << " equity=" << result.equity << '\n';
  } catch (const std::exception& error) {
    std::cerr << "error=" << error.what() << '\n';
    return 2;
  }
}
