#include <iostream>
#include <stdexcept>

#include "quant/capstone/order_book.hpp"

int main() {
  using quant::capstone::LimitOrderBook;
  using quant::capstone::Order;
  using quant::capstone::SequenceGate;
  using quant::capstone::Side;

  LimitOrderBook book;
  book.submit(Order{1, Side::sell, 101, 6});
  book.submit(Order{2, Side::buy, 100, 5});
  const auto trades = book.submit(Order{3, Side::buy, 102, 4});

  SequenceGate replay;
  int accepted = 0;
  int duplicate = 0;
  int gap = 0;
  for (const auto sequence : {1ULL, 2ULL, 2ULL, 4ULL}) {
    switch (replay.observe(sequence)) {
      case quant::capstone::ReplayStatus::accepted:
        ++accepted;
        break;
      case quant::capstone::ReplayStatus::duplicate_or_stale:
        ++duplicate;
        break;
      case quant::capstone::ReplayStatus::gap:
        ++gap;
        break;
      case quant::capstone::ReplayStatus::decode_error:
      case quant::capstone::ReplayStatus::book_rejected:
      case quant::capstone::ReplayStatus::sequence_gap_exceeded:
      case quant::capstone::ReplayStatus::pending_overflow:
        throw std::logic_error{"sequence gate returned a transport status"};
    }
  }

  const auto& trade = trades.front();
  std::cout << "trade price=" << trade.price_ticks
            << " quantity=" << trade.quantity << " maker=" << trade.maker_id
            << " taker=" << trade.taker_id << '\n'
            << "book best-bid=" << *book.best_bid()
            << " best-ask=" << *book.best_ask() << '\n'
            << "replay accepted=" << accepted << " duplicate=" << duplicate
            << " gap=" << gap << " next=" << replay.next_expected() << '\n';
}
