#include <atomic>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "ukl/keyboard.hpp"
#include "ukl/randomize.hpp"

std::atomic<bool> keyboard::running {true};

std::vector<std::vector<unsigned char>> keyboard::random_layout(
  const size_t width, const size_t height,
  std::vector<unsigned char> chars,
  const std::vector<std::pair<size_t, size_t>>& leave_blank
) {
  if (chars.empty() || chars.size() < width * height - leave_blank.size()) {
    throw std::runtime_error("Charset too small to fill the given layout dimensions!");
  }

  std::vector<std::vector<unsigned char>> keys_vec;
  for (size_t i {0}; i < height; ++i) {
    keys_vec.emplace_back(width, 0);
    for (size_t j {0}; j < keys_vec[i].size(); ++j) {
      bool ignore = false;
      for (const auto& pair : leave_blank) {
        if (pair.first == i && pair.second == j) {
          ignore = true;
          break;
        }
      }

      if (ignore) {
        continue;
      }

      if (chars.empty()) {
        throw std::runtime_error("Not enough letters in charset!");
      }

      size_t idx {randomize::random_size_t(0, chars.size() - 1)};
      keys_vec[i][j] = chars[idx];
      chars.erase(chars.begin() + idx);
    }
  }

  return keys_vec;
}

keyboard::optimization::WorkerData::WorkerData(Layout layout) 
: best {std::move(layout)}, 
  cur {best},
  pos_ema_mean(cur.pos_score),
  sf_ema_mean(cur.sf_score),
  flow_ema_mean(cur.flow_score)
{}

void keyboard::optimization::WorkerData::modify(const bool imbalanced) {
  double r {randomize::random_double()};
  if (imbalanced && r < imbalance) {
    return cur.hand_shuffle();
  }

  r -= imbalance;
  if (r < macro) {
    if (r / macro < 0.65) {
      return cur.finger_exchange();
    } else {
      return cur.row_exchange();
    }
  }

  r -= macro;
  if (r < meso) {
    if (r / meso < 0.5) {
      return cur.finger_shuffle();
    } else {
      return cur.row_shuffle();
    }
  }

  if (r / micro < 0.6) {
    return cur.simple_swap();
  } else {
    return cur.hand_shuffle();
  }
}

bool under_threshold(const int64_t score, const int64_t basis, const double factor) {
  int64_t threshold {basis - (static_cast<int64_t>(std::abs(basis) * factor))};
  return score < threshold;
}

void keyboard::optimization::WorkerData::run() {
  bool imbalanced {false};
  cur.score = 
    cur.pos_score + 
    cur.sf_score +
    cur.flow_score;
  Layout base {cur};
  uint64_t stagnation {0};

  while (running.load()) {
    ++stagnation;
    ++num_iterations;

    if (stagnation > 100'000'000) {
      stagnation = 0;
      heat = 0.7;
    } else {
      heat *= 0.9999999;
    }

    modify(imbalanced);

    double score {0};
    int64_t pos_score {cur.compute_pos_score()};
    cur.pos_score = pos_score;
    score += cur.pos_score;
    imbalanced = cur.left_percentage < 40 || cur.left_percentage > 60;

    int64_t sf_score {cur.compute_sf_score()};
    cur.sf_score = sf_score;
    score += cur.sf_score;

    int64_t flow_score {cur.compute_flow_score()};
    cur.flow_score = flow_score;
    score += cur.flow_score;

    double delta {score - base.score};
    if (delta < 0 && randomize::random_double() >= exp(delta / heat)) {
      cur = base;
      continue;
    }

    cur.score = score;
    base = cur;

    if (score > best.score) {
      best = cur;
      stagnation = 0;
    }
  }
}

void keyboard::optimization::WorkerData::print(
  std::ofstream& os,
  const std::unordered_map<unsigned char, std::string>& mapping
) const {
  for (const auto& row : best.keys) {
    for (size_t i {0}; i < row.size(); ++i) {
      const auto& it {mapping.find(row[i])};
      if (it == mapping.end()) {
        throw std::runtime_error(
          "Incomplete mapping for printing! Missing unsigned char: " + 
          std::to_string(row[i])
        );
      }
      os << it->second << ' ';
    }
    os << '\n';
  }

  os.imbue(std::locale(""));
  os << 
    "\n" <<
    "Balance: " << std::fixed << std::setprecision(2) <<
    best.left_percentage * 100 << " | " << (1.0 - best.left_percentage) * 100 << '\n' <<
    std::defaultfloat <<
    "Position score: " << best.pos_score << '\n' <<
    "Same finger score: " << best.sf_score << '\n' <<
    "Flow score: " << best.flow_score << '\n' <<
    "Overall score: " << static_cast<int64_t>(best.score) << "\n\n\n";
}

double keyboard::optimization::WorkerData::normalize(
  const double value,
  const double mean,
  const double var) const {
  return (value - mean) / std::sqrt(std::max(var, 0.0) + 1e-9);
}

void keyboard::optimization::WorkerData::update_ema(
  double& mean,
  double& var,
  const double value
) {
  double delta {value - mean};
  mean += alpha * delta;
  var = (1 - alpha) * (var + alpha * delta * delta);
}

