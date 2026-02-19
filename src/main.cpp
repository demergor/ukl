#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "ukl/conversion.hpp"
#include "ukl/io.hpp"
#include "ukl/keyboard.hpp"
#include "ukl/ngrams.hpp"
#include "ukl/randomize.hpp"

struct ShutdownHook {
public:
  std::vector<keyboard::optimization::WorkerData> workers;
  std::unordered_map<std::string, unsigned char> mapping;

  ~ShutdownHook() {
    auto now {std::chrono::system_clock::now()};
    std::time_t now_time {std::chrono::system_clock::to_time_t(now)};

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time), "%Y_%m_%d"); 
    std::string date_str = oss.str();

    std::filesystem::path result_path {"../resources/results"};
    result_path /= "best_layouts_" + date_str + ".txt";

    std::ofstream output_file {result_path};
    if (!output_file) {
      std::cerr << "Error opening file to write results to: " + result_path.string();
      return;
    }

    for (const auto& worker : workers) {
      worker.print(output_file, mapping);
    }
  }
};

static bool imbalanced {false};

void modify(Layout& layout, const double& move_mode) {
  static constexpr double finger {0.85};
  static constexpr double row {finger + 0.1};
  static constexpr double col {row + 0.05};

  if (imbalanced && move_mode <= 0.8) {
    layout.hand_shuffle();
    imbalanced = false;
    return;
  }

  if (move_mode >= col) {
    layout.column_shuffle();
  } else if (move_mode >= row) {
    layout.row_shuffle();
  } else if (move_mode >= finger) {
    layout.finger_shuffle();
  } else {
    layout.key_pair_shuffle();
  }
}

int optimize(std::atomic<long long>& highscore, Layout& best, double& heat) {
  Layout base {best};
  long long base_score {highscore};

  while (true) {
    long long score {0};
    long long threshold {static_cast<long long>(0.9 * base_score)};
    heat *= 0.995;

    Layout candidate {base};
    modify(candidate, randomize::random_double());

    PositionScore position_score {candidate.compute_position_score()};
    if (position_score.left_percentage < 40 || position_score.left_percentage > 60) {
      imbalanced = true;
    }

    score += position_score.value;
    if (score < threshold) {
      continue;
    }

    score += candidate.compute_same_finger_score();
    if (score < threshold) {
      continue;
    }

    score += candidate.compute_flow_score();
    if (score < threshold) {
      continue;
    }

    long long delta {score - base_score};
    if (delta < 0 && randomize::random_double() >= exp(delta / heat)) {
      continue;
    }

    base = candidate;
    base_score = score;

    if (score > highscore) {
      best = candidate;
      highscore = score;
    }
  }
}

int main() {
  std::set<std::string> letters;
  std::unordered_map<std::string, unsigned char> mapping;

  {
    std::filesystem::path in_path {"../resources/frequencies/letters.txt"};
    std::ifstream in_file {in_path};

    if (!in_file) {
      throw std::runtime_error("Failed to open input file " + in_path.string());
    }

    letters = io::fetch_letters(in_file);
    mapping = conversion::encode(letters);

    std::vector<std::filesystem::path> paths {
      "../resources/frequencies/letters.txt",
      "../resources/frequencies/bigram.txt",
      "../resources/frequencies/skipgram.txt",
      "../resources/frequencies/spacegram.txt",
      "../resources/frequencies/trigram.txt", 

      "../resources/asymmetries/bigram.txt",
      "../resources/asymmetries/skipgram.txt",
      "../resources/asymmetries/spacegram.txt",
      "../resources/asymmetries/trigram.txt", 
    };

    for (std::size_t i {0}; i < paths.size(); ++i) {
      in_file.close();
      in_file.open(paths[i]);

      if (!in_file) {
        throw std::runtime_error("Failed to open input file " + paths[i].string());
      }

      switch (i) {
        case 0: 
          io::fetch_values(Ngrams::char_freqs, in_file, mapping, 10);
          break;
        case 1: 
          io::fetch_values(Ngrams::bigram_freqs, in_file, mapping, 10); 
          break;
        case 2: 
          io::fetch_values(Ngrams::skipgram_freqs, in_file, mapping, 5); 
          break;
        case 3: 
          io::fetch_values(Ngrams::spacegram_freqs, in_file, mapping, 2); 
          break;
        case 4: 
          io::fetch_values(Ngrams::trigram_freqs, in_file, mapping, 10); 
          break;
        case 5: 
          io::fetch_values(Ngrams::bigram_asyms, in_file, mapping, 10); 
          break;
        case 6: 
          io::fetch_values(Ngrams::skipgram_asyms, in_file, mapping, 5); 
          break;
        case 7: 
          io::fetch_values(Ngrams::spacegram_asyms, in_file, mapping, 2); 
          break;
        case 8: 
          io::fetch_values(Ngrams::trigram_asyms, in_file, mapping, 10); 
          break;
      }
    }
  }

  std::vector<unsigned char> encoded_letters;
  for (const auto& str : letters) {
    auto it {mapping.find(str)};
    if (it == mapping.end()) {
      throw std::runtime_error("Incomplete mapping: " + str + " missing!");
    }

    encoded_letters.emplace_back(it->second);
  }

  std::size_t num_threads {
    std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4
  };

  std::vector<keyboard::optimization::WorkerData> workers;
  std::vector<std::pair<const size_t, const size_t>> leave_blank {
      {{3, 6}, {3, 7}, {4, 5}, {4, 6}, {4, 7}, {4, 8}},
  };
  std::vector<std::vector<size_t>> finger_groupings {
    {0, 1},
    {2},
    {3},
    {4, 5, 6},
    {7, 8, 9}, 
    {10},
    {11},
    {12, 13}
  };

  for (std::size_t i {0}; i < num_threads; ++i) {
    Layout layout {
      5, 14,
      encoded_letters,
      leave_blank,
      finger_groupings
    };

    // TODO: Fill out other layout data
    keyboard::optimization::WorkerData worker {layout};
    workers.emplace_back(worker);
    // worker.run();
  }

  static ShutdownHook sh {workers, mapping};

  return 0;
}
