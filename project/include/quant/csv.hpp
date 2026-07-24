#pragma once

#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "quant/types.hpp"

namespace quant {

enum class MarketEventField : std::uint8_t {
  header,
  row,
  timestamp,
  symbol,
  price,
  quantity,
  stream,
};

enum class ParseErrorCode : std::uint8_t {
  missing_header,
  unexpected_header,
  wrong_column_count,
  empty_value,
  invalid_number,
  out_of_range,
  non_finite,
  non_positive,
  read_failure,
};

struct ParseError final {
  std::size_t line{};
  MarketEventField field{MarketEventField::row};
  ParseErrorCode code{ParseErrorCode::invalid_number};
};

struct MarketEventReadResult final {
  std::vector<MarketEvent> events;
  std::optional<ParseError> error;

  [[nodiscard]] bool has_value() const noexcept { return !error.has_value(); }
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
inline std::optional<ParseErrorCode> parse_number(std::string_view text,
                                                  Number& value) {
  const char* begin = text.data();
  const char* end = begin + text.size();
  const auto [position, error] = std::from_chars(begin, end, value);
  if (error == std::errc::result_out_of_range) {
    return ParseErrorCode::out_of_range;
  }
  if (error != std::errc{} || position != end) {
    return ParseErrorCode::invalid_number;
  }
  return std::nullopt;
}

inline void remove_carriage_return(std::string& row) {
  if (!row.empty() && row.back() == '\r') {
    row.pop_back();
  }
}

}  // namespace detail

inline std::string_view field_name(MarketEventField field) {
  switch (field) {
    case MarketEventField::header:
      return "header";
    case MarketEventField::row:
      return "row";
    case MarketEventField::timestamp:
      return "timestamp";
    case MarketEventField::symbol:
      return "symbol";
    case MarketEventField::price:
      return "price";
    case MarketEventField::quantity:
      return "quantity";
    case MarketEventField::stream:
      return "stream";
  }
  return "unknown";
}

inline std::string_view error_code_name(ParseErrorCode code) {
  switch (code) {
    case ParseErrorCode::missing_header:
      return "missing-header";
    case ParseErrorCode::unexpected_header:
      return "unexpected-header";
    case ParseErrorCode::wrong_column_count:
      return "wrong-column-count";
    case ParseErrorCode::empty_value:
      return "empty-value";
    case ParseErrorCode::invalid_number:
      return "invalid-number";
    case ParseErrorCode::out_of_range:
      return "out-of-range";
    case ParseErrorCode::non_finite:
      return "non-finite";
    case ParseErrorCode::non_positive:
      return "non-positive";
    case ParseErrorCode::read_failure:
      return "read-failure";
  }
  return "unknown";
}

inline std::string format_parse_error(const ParseError& error) {
  return "line " + std::to_string(error.line) + ": " +
         std::string{field_name(error.field)} + " " +
         std::string{error_code_name(error.code)};
}

inline MarketEventReadResult read_market_events(std::istream& input) {
  MarketEventReadResult result;
  std::string row;
  if (!std::getline(input, row)) {
    result.error = ParseError{1, MarketEventField::header,
                              input.bad() ? ParseErrorCode::read_failure
                                          : ParseErrorCode::missing_header};
    return result;
  }
  detail::remove_carriage_return(row);
  if (row != "timestamp_ms,symbol,price,quantity") {
    result.error = ParseError{1, MarketEventField::header,
                              ParseErrorCode::unexpected_header};
    return result;
  }

  std::size_t line = 1;
  while (std::getline(input, row)) {
    ++line;
    detail::remove_carriage_return(row);
    if (row.empty()) {
      continue;
    }
    std::array<std::string_view, 4> fields{};
    std::int64_t timestamp_ms = 0;
    std::int64_t quantity = 0;
    double price = 0.0;
    if (!detail::split_row(row, fields)) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::row,
                                ParseErrorCode::wrong_column_count};
      return result;
    }
    if (fields[1].empty()) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::symbol,
                                ParseErrorCode::empty_value};
      return result;
    }
    if (const auto error = detail::parse_number(fields[0], timestamp_ms)) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::timestamp, *error};
      return result;
    }
    if (const auto error = detail::parse_number(fields[2], price)) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::price, *error};
      return result;
    }
    if (!std::isfinite(price)) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::price,
                                ParseErrorCode::non_finite};
      return result;
    }
    if (price <= 0.0) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::price,
                                ParseErrorCode::non_positive};
      return result;
    }
    if (const auto error = detail::parse_number(fields[3], quantity)) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::quantity, *error};
      return result;
    }
    if (quantity <= 0) {
      result.events.clear();
      result.error = ParseError{line, MarketEventField::quantity,
                                ParseErrorCode::non_positive};
      return result;
    }
    result.events.push_back(MarketEvent{
        Timestamp{std::chrono::milliseconds{timestamp_ms}},
        std::string{fields[1]}, price, quantity});
  }
  if (input.bad()) {
    result.events.clear();
    result.error = ParseError{line + 1, MarketEventField::stream,
                              ParseErrorCode::read_failure};
  }
  return result;
}

}  // namespace quant
