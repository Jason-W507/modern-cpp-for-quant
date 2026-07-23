#include "quant/capstone/factor_kernel.hpp"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

double read_number(std::istream& input) {
  std::string token;
  if (!(input >> token)) {
    throw std::runtime_error{"unexpected end of factor batch"};
  }
  std::size_t consumed{};
  const double value = std::stod(token, &consumed);
  if (consumed != token.size()) {
    throw std::runtime_error{"invalid numeric token: " + token};
  }
  return value;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "usage: capstone_factor_kernel_cli BATCH_FILE\n";
    return 2;
  }
  try {
    std::ifstream input{argv[1]};
    std::size_t rows{};
    std::size_t columns{};
    if (!(input >> rows >> columns) || rows == 0 || columns == 0) {
      throw std::runtime_error{"invalid factor batch dimensions"};
    }
    std::vector<double> weights(columns);
    for (double& weight : weights) {
      weight = read_number(input);
    }
    std::vector<double> values(rows * columns);
    for (double& value : values) {
      value = read_number(input);
    }
    std::string trailing;
    if (input >> trailing) {
      throw std::runtime_error{"trailing factor batch data"};
    }

    const auto scores = quant::capstone::weighted_factor(
        quant::capstone::FactorBatchView{values, rows, columns}, weights);
    std::cout << std::setprecision(17);
    for (std::size_t index = 0; index < scores.size(); ++index) {
      if (index != 0) {
        std::cout << ',';
      }
      std::cout << scores[index];
    }
    std::cout << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "factor batch error: " << error.what() << '\n';
    return 2;
  }
}
