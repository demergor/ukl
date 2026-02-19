#include <cstddef>
#include <random>

#include "ukl/randomize.hpp"

double randomize::random_double() {
  thread_local static std::mt19937 rng {std::random_device {}()};
  thread_local static std::uniform_real_distribution<double> dist {0.0, 1.0};
  return dist(rng);
}

size_t randomize::random_size_t(const size_t incl_floor, const size_t incl_ceiling) {
  thread_local static std::mt19937 rng {std::random_device {}()};
  std::uniform_int_distribution<size_t> dist {incl_floor, incl_ceiling};
  return dist(rng);
}
