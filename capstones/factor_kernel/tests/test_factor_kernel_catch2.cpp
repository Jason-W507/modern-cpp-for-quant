#include "quant/capstone/factor_kernel.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <cmath>
#include <vector>

struct WorkedFactorFixture {
  std::vector<double> values{1.0, 2.0, NAN};
  std::vector<double> weights{0.5, -0.25, 0.25};
};

TEST_CASE("factor kernel rejects malformed public batches") {
  const auto [values, rows, columns, weights] = GENERATE(
      table<std::vector<double>, std::size_t, std::size_t, std::vector<double>>({
          {{1.0, 2.0}, 2, 2, {1.0, 1.0}},
          {{1.0, 2.0}, 1, 2, {1.0}},
          {{INFINITY}, 1, 1, {1.0}},
      }));
  REQUIRE_THROWS_AS(
      quant::capstone::weighted_factor({values, rows, columns}, weights),
      std::invalid_argument);
}

TEST_CASE_METHOD(WorkedFactorFixture, "factor fixture preserves the worked value",
                 "[fixture]") {
  const auto result = quant::capstone::weighted_factor({values, 1, 3}, weights);
  REQUIRE(result.size() == 1);
  REQUIRE(result.front() == Catch::Approx(0.0).margin(1e-12));
}
