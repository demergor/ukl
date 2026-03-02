#ifndef IO_HPP
#define IO_HPP

#include <cassert>
#include <climits>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

#include "ukl/utf8.hpp"

namespace io {

std::filesystem::path resolve(const std::string&);
std::set<std::string> fetch_letters(std::ifstream& in);
template <std::integral IntType, size_t N>
void fetch_values(
  std::array<IntType, N>& out,
  std::ifstream& in,
  const std::unordered_map<std::string, unsigned char>& mapping,
  const int multiplier
) {

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) {
      continue;
    }

    size_t delim_pos {line.find(" : ")};
    if (delim_pos == std::string::npos) {
      continue;
    }

    size_t index {0};
    size_t out_index {0};

    bool skip {false};
    while (index < delim_pos && line[index] != ' ') {
      assert(index < line.size());

      size_t offset {utf8::num_bytes(line[index])};
      if (index + offset > delim_pos) {
        throw std::runtime_error("Malformed UTF-8 input!");
      }

      auto encoded {mapping.find(std::string(&line[index], offset))};
      if (encoded == mapping.end()) {
        // If not all letters can or should be processed
        skip = true;
        break;
      }

      out_index = (out_index << sizeof(unsigned char) * CHAR_BIT) | encoded->second;
      if (out_index >= N) {
        throw std::runtime_error(
          "Insufficient space in output array! (" +
          std::to_string(out_index) +
          " -> " +
          std::to_string(N) +
          ")"
        );
      }

      index += offset;
    }

    if (skip) {
      continue;
    }

    const char* num_start {line.data() + delim_pos +  3};
    const char* num_end {line.data() + line.size()};

    while (
      num_start < num_end &&
      std::isspace(static_cast<unsigned char>(*num_start))
    ) {
      ++num_start;
    }

    while (
      num_end > num_start &&
      std::isspace(static_cast<unsigned char>(*(num_end - 1)))
    ) {
      --num_end;
    }

    if (num_start >= num_end) {
      throw std::runtime_error("No numeric value to parse!\nLine contents: " + line);
    }

    auto fc_res {std::from_chars(num_start, num_end, out.at(out_index))};
    if (fc_res.ec != std::errc{}) {
      throw std::runtime_error("Failed to parse number!\nLine contents: " + line);
    }

    out[out_index] *= multiplier;
  }
}

}

#endif
