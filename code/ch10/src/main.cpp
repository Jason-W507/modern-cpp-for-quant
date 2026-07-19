#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#include "quant/ch10/pipeline.hpp"

int run(const std::string& path) {
  std::ifstream input{path};
  if (!input) {
    throw std::runtime_error{"cannot open input file"};
  }

  quant::ch10::Portfolio portfolio{10'000.0};
  const auto metrics = quant::ch10::process_fill_csv(input, std::cerr, portfolio);
  const auto snapshot = portfolio.snapshot("AAPL", 99.0);
  std::cout << std::fixed << std::setprecision(2)
            << "accepted=" << metrics.rows_accepted
            << " rejected=" << metrics.rows_rejected
            << " committed=" << metrics.batches_committed
            << " rolled_back=" << metrics.batches_rolled_back
            << " quantity=" << snapshot.quantity << " cash=" << snapshot.cash
            << " equity=" << snapshot.equity << '\n';
  return 0;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "fatal context=cli message=expected one CSV path\n";
    return 2;
  }
  try {
    return run(argv[1]);
  } catch (const std::exception& exception) {
    std::cerr << "fatal context=" << argv[1] << " message=" << exception.what()
              << '\n';
    return 2;
  }
}
