#include <cstddef>
#include <random>

namespace randomize {

thread_local static std::mt19937 rng {std::random_device{}()};

inline double random_double() {
  thread_local static std::uniform_real_distribution<double> dist {0.0, 1.0};
  return dist(rng);
}

inline size_t random_size_t(const size_t min, const size_t max) {
  std::uniform_int_distribution<size_t> dist {min, max};
  return dist(rng);
}

}
