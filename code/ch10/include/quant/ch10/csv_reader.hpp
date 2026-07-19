#ifndef QUANT_CH10_CSV_READER_HPP
#define QUANT_CH10_CSV_READER_HPP

#include <cstddef>
#include <string>
#include <variant>

#include "quant/ch10/domain.hpp"

namespace quant::ch10 {

struct ParseError final {
  std::size_t line{};
  std::string field;
  std::string message;
};

using FillParseResult = std::variant<Fill, ParseError>;

FillParseResult parse_fill_row(const std::string& row, std::size_t line);

}  // namespace quant::ch10

#endif
