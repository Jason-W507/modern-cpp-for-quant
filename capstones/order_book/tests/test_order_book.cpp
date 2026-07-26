#include <iostream>
#include <stdexcept>

#include "quant/capstone/order_book.hpp"

int main() {
  using quant::capstone::LimitOrderBook;
  using quant::capstone::Order;
  using quant::capstone::ReplayStatus;
  using quant::capstone::SequenceGate;
  using quant::capstone::Side;

  LimitOrderBook book;
  book.submit(Order{1, Side::sell, 101, 6});
  book.submit(Order{2, Side::buy, 100, 5});
  const auto trades = book.submit(Order{3, Side::buy, 102, 4});
  if (trades.size() != 1 || trades[0].maker_id != 1 ||
      trades[0].taker_id != 3 || trades[0].price_ticks != 101 ||
      trades[0].quantity != 4 || book.best_bid() != 100 ||
      book.best_ask() != 101) {
    std::cerr << "price-time oracle mismatch\n";
    return 2;
  }

  bool duplicate_rejected = false;
  try {
    book.submit(Order{2, Side::buy, 99, 1});
  } catch (const std::invalid_argument&) {
    duplicate_rejected = true;
  }
  SequenceGate replay;
  const bool replay_ok = replay.observe(1) == ReplayStatus::accepted &&
                         replay.observe(2) == ReplayStatus::accepted &&
                         replay.observe(2) == ReplayStatus::duplicate_or_stale &&
                         replay.observe(4) == ReplayStatus::gap &&
                         replay.next_expected() == 3;
  if (!duplicate_rejected || !replay_ok) {
    std::cerr << "replay oracle mismatch\n";
    return 2;
  }
  bool invalid_side_rejected = false;
  try {
    book.submit(Order{4, static_cast<Side>(42), 100, 1});
  } catch (const std::invalid_argument&) {
    invalid_side_rejected = true;
  }
  bool invalid_query_rejected = false;
  try {
    (void)book.quantity_at(static_cast<Side>(42), 100);
  } catch (const std::invalid_argument&) {
    invalid_query_rejected = true;
  }
  if (!invalid_side_rejected || !invalid_query_rejected) {
    std::cerr << "side validation mismatch\n";
    return 2;
  }
  std::cout << "order-book-tests-ok trades=" << trades.size()
            << " best-bid=" << *book.best_bid()
            << " best-ask=" << *book.best_ask()
            << " replay-next=" << replay.next_expected() << '\n';
}
