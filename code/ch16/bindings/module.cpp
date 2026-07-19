#include "quant/ch16/numerics.hpp"

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <cstddef>
#include <variant>

namespace py = pybind11;

namespace {

quant::ch16::DType dtype_from(const py::buffer_info& info) {
  if (info.format == py::format_descriptor<double>::format()) {
    return quant::ch16::DType::float64;
  }
  if (info.format == py::format_descriptor<float>::format()) {
    return quant::ch16::DType::float32;
  }
  return quant::ch16::DType::int64;
}

quant::ch16::BatchView as_batch_view(const py::buffer_info& info) {
  if (info.ndim != 1) {
    throw py::value_error{"expected one-dimensional array"};
  }
  return {info.ptr, static_cast<std::size_t>(info.shape[0]), dtype_from(info),
          static_cast<std::ptrdiff_t>(info.strides[0])};
}

double unwrap_sum(const quant::ch16::BatchSum& result) {
  if (const auto* value = std::get_if<double>(&result)) {
    return *value;
  }
  const auto error = std::get<quant::ch16::BoundaryError>(result);
  if (error == quant::ch16::BoundaryError::wrong_dtype) {
    throw py::type_error{"expected dtype float64 without implicit forcecast"};
  }
  if (error == quant::ch16::BoundaryError::non_contiguous) {
    throw py::value_error{"expected C-contiguous one-dimensional array"};
  }
  throw py::value_error{quant::ch16::boundary_error_name(error).data()};
}

double sum_returns(const py::array& values) {
  const py::buffer_info info = values.request();
  const quant::ch16::BatchView view = as_batch_view(info);
  quant::ch16::BatchSum result;
  {
    py::gil_scoped_release release;
    result = quant::ch16::sum_batch(view);
  }
  return unwrap_sum(result);
}

py::array borrowed_view(const py::array& values) {
  const py::buffer_info info = values.request();
  const quant::ch16::BatchSum validation =
      quant::ch16::sum_batch(as_batch_view(info));
  (void)unwrap_sum(validation);
  return py::array{py::dtype::of<double>(), info.shape, info.strides, info.ptr,
                   values};
}

py::array_t<double> centered_copy(const py::array& values) {
  const py::buffer_info info = values.request();
  const quant::ch16::BatchView view = as_batch_view(info);
  const double total = unwrap_sum(quant::ch16::sum_batch(view));
  if (view.size == 0) {
    return py::array_t<double>{0};
  }
  py::array_t<double> output{view.size};
  py::buffer_info output_info = output.request();
  const auto* input_data = static_cast<const double*>(view.data);
  auto* output_data = static_cast<double*>(output_info.ptr);
  const double mean = total / static_cast<double>(view.size);
  {
    py::gil_scoped_release release;
    for (std::size_t index = 0; index < view.size; ++index) {
      output_data[index] = input_data[index] - mean;
    }
  }
  return output;
}

}  // namespace

PYBIND11_MODULE(quant_ch16, module) {
  module.doc() = "Chapter 16 strict batch boundary";
  module.def("sum_returns", &sum_returns);
  module.def("borrowed_view", &borrowed_view);
  module.def("centered_copy", &centered_copy);
}
