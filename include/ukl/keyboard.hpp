#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "ukl/layout.hpp"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <unordered_map>

namespace keyboard {

std::vector<std::vector<unsigned char>> random_layout(
  const size_t, const size_t,
  std::vector<unsigned char>,
  const std::vector<std::pair<const size_t, const size_t>>&
);

namespace optimization {

struct WorkerData {
public:
  static constexpr int64_t initial_max {std::numeric_limits<int64_t>::lowest()};
  static constexpr int64_t initial_min {std::numeric_limits<int64_t>::max()};

  Layout best;
  std::vector<std::pair<Layout, int64_t>> top_ten;
  int64_t 
  highscore {initial_max},
  max_position_score {initial_max},
  max_frequency_score {initial_max},
  max_flow_score {initial_max},
  min_position_score {initial_min},
  min_frequency_score {initial_min},
  min_flow_score {initial_min};

  void print(std::ofstream&, const std::unordered_map<std::string, unsigned char>&) const;

private: 
  double normalize(const int64_t score, const int64_t max, const int64_t min) const;
};

void run(std::atomic<long long>&, Layout&, const double&);

}

}

#endif
