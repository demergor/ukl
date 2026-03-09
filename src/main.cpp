#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstddef>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
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
  std::vector<keyboard::optimization::WorkerData>& workers;
  std::unordered_map<unsigned char, std::string>& mapping;

  ~ShutdownHook() {
    auto now {std::chrono::system_clock::now()};
    std::time_t now_time {std::chrono::system_clock::to_time_t(now)};

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time), "%Y_%m_%d"); 
    std::string date_str = oss.str();

    std::filesystem::path result_path {"resources/results"};
    result_path /= "best_layouts_" + date_str + ".txt";

    std::ofstream output_file {result_path};
    if (!output_file) {
      std::cerr << "Error opening file to write results to: " + result_path.string();
      return;
    }

    // TODO: Sort by layout scores first
    std::sort(
      workers.begin(),
      workers.end(),
      [] (const auto& a, const auto& b) {
        return a.best.score > b.best.score;
      }
    );

    uint64_t num_iterations {0};
    for (const auto& worker : workers) {
      worker.print(output_file, mapping);
      num_iterations += worker.num_iterations;
    }

    std::cout << '\n' << num_iterations << " iterations" << std::endl;
  }
};

ShutdownHook* global_sh {nullptr};
void signal_handler(int signum) {
  keyboard::running.store(false);
}

int main() {
  std::signal(SIGINT, signal_handler);

  std::set<std::string> letters;
  std::unordered_map<std::string, unsigned char> mapping;
  std::unordered_map<unsigned char, std::string> rev_map;

  std::cout << "Generating layouts...\n";
  {
    std::filesystem::path in_path {"resources/frequencies/letter.txt"};
    std::ifstream in_file {in_path};

    if (!in_file) {
      throw std::runtime_error("Failed to open input file " + in_path.string());
    }

    letters = io::fetch_letters(in_file);
    mapping = conversion::encode(letters);
    rev_map = conversion::rev_map(mapping);

    std::vector<std::filesystem::path> paths {
      "resources/frequencies/letter.txt",
      "resources/frequencies/bigram.txt",
      "resources/frequencies/skipgram.txt",
      "resources/frequencies/spacegram.txt",
      "resources/frequencies/trigram.txt", 

      "resources/asymmetries/bigram.txt",
      "resources/asymmetries/skipgram.txt",
      "resources/asymmetries/spacegram.txt",
      "resources/asymmetries/trigram.txt", 
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

  std::size_t num_threads =
    std::thread::hardware_concurrency() ? std::thread::hardware_concurrency(): 4;

  std::vector<keyboard::optimization::WorkerData> workers;
  std::vector<std::pair<size_t, size_t>> leave_blank {
      {3, 6}, {3, 7}, {4, 5}, {4, 6}, {4, 7}, {4, 8},
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
  std::vector<std::thread> threads;

  for (std::size_t i {0}; i < num_threads; ++i) {
    Layout layout {
      14, 5,
      encoded_letters,
      leave_blank,
      finger_groupings
    };

    layout.pos_score = layout.compute_pos_score();
    layout.sf_score = layout.compute_sf_score();
    layout.flow_score = layout.compute_flow_score();
    layout.score = 
      layout.pos_score +
      layout.sf_score +
      layout.flow_score;

    keyboard::optimization::WorkerData worker {std::move(layout)};
    workers.emplace_back(std::move(worker));
  }

  for (size_t i {0}; i < num_threads; ++i) {
    threads.emplace_back([&workers, i] () { workers[i].run(); });
  }

  ShutdownHook sh {workers, rev_map};
  global_sh = &sh;

  for (auto& t : threads) {
    t.join();
  }

  return 0;
}
