#ifndef QUANT_CH04_CSV_STATS_HPP
#define QUANT_CH04_CSV_STATS_HPP

#include <istream>
#include <string>
#include <vector>

namespace quant::ch04 {

bool read_market_csv(std::istream& input, std::vector<std::string>& symbols,
                     std::vector<double>& prices,
                     std::vector<int>& quantities, std::string& error);

int total_quantity(const std::vector<int>& quantities);

bool total_notional(const std::vector<double>& prices,
                    const std::vector<int>& quantities, double& total);

}  // namespace quant::ch04

#endif
