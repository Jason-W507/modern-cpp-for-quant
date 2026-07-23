#include "quant/capstone/factor_kernel.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <algorithm>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <vector>

namespace py = pybind11;

PYBIND11_MODULE(quant_factor_kernel, module) {
  module.def(
      "weighted_factor",
      [](py::array_t<double, py::array::c_style> values,
         py::array_t<double, py::array::c_style> weights) {
        const auto value_info = values.request();
        const auto weight_info = weights.request();
        if (value_info.ndim != 2 || weight_info.ndim != 1 ||
            value_info.shape[1] != weight_info.shape[0]) {
          throw std::invalid_argument{"expected values[rows, columns] and weights[columns]"};
        }
        const auto rows = static_cast<std::size_t>(value_info.shape[0]);
        const auto columns = static_cast<std::size_t>(value_info.shape[1]);
        const auto scores = quant::capstone::weighted_factor(
            quant::capstone::FactorBatchView{
                std::span<const double>{
                    static_cast<const double*>(value_info.ptr), rows * columns},
                rows, columns},
            std::span<const double>{
                static_cast<const double*>(weight_info.ptr), columns});

        py::array_t<double> output(scores.size());
        auto output_info = output.request();
        std::copy(scores.begin(), scores.end(),
                  static_cast<double*>(output_info.ptr));
        return output;
      },
      py::arg("values").noconvert(), py::arg("weights").noconvert(),
      "Compute one missing-aware weighted factor value per row.");
}
