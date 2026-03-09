#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "ukl/layout.hpp"

#include <atomic>
#include <cstdint>
#include <fstream>
#include <unordered_map>

namespace keyboard {

extern std::atomic<bool> running;

std::vector<std::vector<unsigned char>> random_layout(
  const size_t, const size_t,
  std::vector<unsigned char>,
  const std::vector<std::pair<size_t, size_t>>&
);

namespace optimization {

struct WorkerData {
public:
  Layout best, cur;
  uint64_t num_iterations {0};
  double 
  pos_ema_mean,
  sf_ema_mean,
  flow_ema_mean,

  pos_ema_var {1},
  sf_ema_var {1},
  flow_ema_var {1},

  heat {1000},
  micro {0.50},
  meso {0.2},
  macro {0.1},
  imbalance {0.2},
  alpha {1e-6};


  explicit WorkerData(Layout);

  void modify(const bool);
  void run();
  void print(std::ofstream&, const std::unordered_map<unsigned char, std::string>&) const;

private: 
  double normalize(const double value, const double mean, const double var) const;
  void update_ema(double& mean, double& var, const double value);
};

void run(std::atomic<long long>&, Layout&, const double&);

}

}

#endif
