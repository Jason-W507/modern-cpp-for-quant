#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numeric>
#include <span>
#include <vector>

#if __has_include(<experimental/simd>)
#include <experimental/simd>
#define QUANT_HAS_EXPERIMENTAL_SIMD 1
#else
#define QUANT_HAS_EXPERIMENTAL_SIMD 0
#endif

namespace {

double scalar_dot(std::span<const double> left,
                  std::span<const double> right) {
  return std::inner_product(left.begin(), left.end(), right.begin(), 0.0);
}

double simd_dot(std::span<const double> left, std::span<const double> right) {
#if QUANT_HAS_EXPERIMENTAL_SIMD
  namespace sx = std::experimental;
  using Pack = sx::native_simd<double>;
  Pack sum{};
  std::size_t index{};
  for (; index + Pack::size() <= left.size(); index += Pack::size()) {
    const Pack left_pack{left.data() + index, sx::element_aligned};
    const Pack right_pack{right.data() + index, sx::element_aligned};
    sum += left_pack * right_pack;
  }
  double result = sx::reduce(sum);
  for (; index < left.size(); ++index) {
    result += left[index] * right[index];
  }
  return result;
#else
  return scalar_dot(left, right);
#endif
}

}  // namespace

int main() {
  std::vector<double> values(1'027);
  std::vector<double> weights(values.size());
  std::iota(values.begin(), values.end(), 1.0);
  std::transform(values.begin(), values.end(), weights.begin(),
                 [](double value) { return 1.0 / value; });
  const double scalar = scalar_dot(values, weights);
  const double candidate = simd_dot(values, weights);
  if (std::abs(scalar - candidate) > 1e-10) {
    std::cerr << "SIMD oracle mismatch\n";
    return 2;
  }
#if QUANT_HAS_EXPERIMENTAL_SIMD
  constexpr std::size_t width = std::experimental::native_simd<double>::size();
#else
  constexpr std::size_t width = 1;
#endif
  std::cout << "simd-demo-ok elements=" << values.size() << " width=" << width
            << " checksum=" << candidate << '\n';
}
