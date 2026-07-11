#include <iostream>
#include <stdexcept>
#include <vector>

[[nodiscard]] double final_marked_equity(const std::vector<double>& prices,
                                         double initial_cash) {
  if (prices.empty()) {
    throw std::invalid_argument("prices must not be empty");
  }
  return initial_cash + (prices.back() - prices.front());
}

#ifdef QUANT_WITH_PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

PYBIND11_MODULE(quant_boundary, module) {
  module.def("final_marked_equity", &final_marked_equity,
             pybind11::arg("prices"), pybind11::arg("initial_cash"));
}
#else
int main() {
  const std::vector<double> prices{99.0, 101.0, 103.0};
  std::cout << "pybind-boundary final-equity="
            << final_marked_equity(prices, 99.0) << '\n';
}
#endif
