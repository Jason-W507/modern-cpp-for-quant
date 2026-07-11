#pragma once

#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

#include "quant/types.hpp"

namespace quant {

struct MarketEventReadResult final {
  std::vector<MarketEvent> events;
  std::string error;

  [[nodiscard]] bool has_value() const noexcept { return error.empty(); }
};

namespace detail {

inline bool split_row(std::string_view row,
                      std::array<std::string_view, 4>& fields) {
  for (std::size_t index = 0; index < fields.size(); ++index) {
    const auto comma = row.find(',');
    if (index + 1 == fields.size()) {
      if (comma != std::string_view::npos) {
        return false;
      }
      fields[index] = row;
      return true;
    }
    if (comma == std::string_view::npos) {
      return false;
    }
    fields[index] = row.substr(0, comma);
    row.remove_prefix(comma + 1);
  }
  return true;
}

template <typename Number>
inline bool parse_number(std::string_view text, Number& value) {
  const char* begin = text.data();
  const char* end = begin + text.size();
  const auto [position, error] = std::from_chars(begin, end, value);
  return error == std::errc{} && position == end;
}

}  // namespace detail

inline MarketEventReadResult read_market_events(std::istream& input) {
  MarketEventReadResult result;
  std::string row;
  if (!std::getline(input, row)) {
    result.error = "line 1: missing CSV header";
    return result;
  }
  if (row != "timestamp_ms,symbol,price,quantity") {
    result.error = "line 1: expected timestamp_ms,symbol,price,quantity";
    return result;
  }

  std::size_t line = 1;
  while (std::getline(input, row)) {
    ++line;
    if (row.empty()) {
      continue;
    }
    std::array<std::string_view, 4> fields{};
    std::int64_t timestamp_ms = 0;
    std::int64_t quantity = 0;
    double price = 0.0;
    if (!detail::split_row(row, fields) || fields[1].empty() ||
        !detail::parse_number(fields[0], timestamp_ms) ||
        !detail::parse_number(fields[2], price) ||
        !detail::parse_number(fields[3], quantity) || !std::isfinite(price) ||
        price <= 0.0 || quantity <= 0) {
      result.events.clear();
      result.error = "line " + std::to_string(line) + ": invalid market event";
      return result;
    }
    result.events.push_back(MarketEvent{
        Timestamp{std::chrono::milliseconds{timestamp_ms}},
        std::string{fields[1]}, price, quantity});
  }
  return result;
}

}  // namespace quant
