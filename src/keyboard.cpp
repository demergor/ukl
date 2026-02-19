#include <cstddef>
#include <stdexcept>
#include <vector>

#include "ukl/keyboard.hpp"
#include "ukl/randomize.hpp"

std::vector<std::vector<unsigned char>> keyboard::random_layout(
  const size_t width, const size_t height,
  std::vector<unsigned char> chars,
  const std::vector<std::pair<const size_t, const size_t>>& leave_blank
) {
  if (chars.size() < width * height - leave_blank.size()) {
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

      size_t idx {randomize::random_size_t(0, chars.size() - 1)};
      keys_vec[i][j] = chars[idx];
      chars.erase(chars.begin() + idx);
    }
  }

  return keys_vec;
}

double keyboard::optimization::WorkerData::normalize(
  const int64_t score,
  const int64_t max,
  const int64_t min
) const {
  if (max == min) {
    return 0.5;
  }

  return (static_cast<double>(score - min)) / (max - min);
}
