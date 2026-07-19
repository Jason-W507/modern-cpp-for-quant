#ifndef QUANT_CH10_PIPELINE_HPP
#define QUANT_CH10_PIPELINE_HPP

#include <cstddef>
#include <iosfwd>

#include "quant/ch10/portfolio.hpp"

namespace quant::ch10 {

struct ProcessingMetrics final {
  std::size_t rows_seen{};
  std::size_t rows_accepted{};
  std::size_t rows_rejected{};
  std::size_t batches_committed{};
  std::size_t batches_rolled_back{};
};

ProcessingMetrics process_fill_csv(std::istream& input,
                                   std::ostream& log,
                                   Portfolio& portfolio);

}  // namespace quant::ch10

#endif
