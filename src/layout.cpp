#include <cassert>
#include <climits>
#include <cstddef>
#include <stdexcept>

#include "ukl/keyboard.hpp"
#include "ukl/layout.hpp"
#include "ukl/ngrams.hpp"
#include "ukl/randomize.hpp"

Layout::Layout(
  const size_t width, const size_t height,
  std::vector<unsigned char> chars,
  const std::vector<std::pair<const size_t, const size_t>>& leave_blank,
  const std::vector<std::vector<size_t>>& finger_groupings
) : keys {keyboard::random_layout(width, height, chars, leave_blank)} {
  for (const auto& finger : finger_groupings) {
    std::vector<size_t*> finger_idxs;
    for (const auto& col : finger) {
      for (size_t i {0}; i < keys.size(); ++i) {
        finger_idxs.emplace_back(&keys[i][col]);
      }
    }
  }
}

void Layout::column_exchange() {
  assert(!keys.empty());
  assert(!keys[0].empty());

  size_t max_col_idx {keys[0].size() - 1};
  size_t lcol_idx {randomize::random_size_t(0, max_col_idx--)};
  size_t rcol_idx {randomize::random_size_t(0, max_col_idx)};

  if (rcol_idx >= lcol_idx) {
    ++rcol_idx;
  }

  for (size_t i {0}; i < keys.size(); ++i) {
    if (keys[i][lcol_idx] && keys[i][rcol_idx]) {
      std::swap(keys[i][lcol_idx], keys[i][rcol_idx]);
    }
  }
}

void Layout::column_shuffle() {
  assert(!keys.empty());
  assert(!keys[0].empty());

  size_t max_col_idx {keys[0].size() - 1};
  size_t col_idx {randomize::random_size_t(0, max_col_idx)};

  std::vector<size_t> non_empty;
  non_empty.reserve(keys.size());

  for (size_t i {0}; i < keys.size(); ++i) {
    if (keys[i][col_idx]) {
      non_empty.emplace_back(i);
    }
  }

  size_t max_idx {non_empty.size() - 1};
  for (size_t i {0}; i <= max_idx; ++i) {
    size_t swap_idx {randomize::random_size_t (0, max_idx)};
    std::swap(keys[non_empty[i]][col_idx], keys[non_empty[swap_idx]][col_idx]);
  }
}

void Layout::hand_shuffle() {
  assert(!keys.empty());

  size_t max_row_idx {keys.size() - 1};
  size_t lrow_idx {randomize::random_size_t(0, max_row_idx)};
  size_t rrow_idx {randomize::random_size_t(0, max_row_idx)};

  std::vector<size_t> non_empty;
  non_empty.reserve(8);

  for (size_t i {0}; i < keys[lrow_idx].size(); ++i) {
    if (keys[lrow_idx][i]) {
      non_empty.emplace_back(keys[lrow_idx][i]);
    }
  }

  if (non_empty.empty()) {
    throw std::runtime_error("No non-empty cells to pick from for the left hand!");
  }

  size_t lcol_idx {non_empty[randomize::random_size_t(0, non_empty.size() - 1)]};
  non_empty.clear();

  for (size_t i {0}; i < keys[rrow_idx].size(); ++i) {
    if (keys[rrow_idx][i]) {
      non_empty.emplace_back(keys[rrow_idx][i]);
    }
  }

  if (non_empty.empty()) {
    throw std::runtime_error("No non-empty cells to pick from for the right hand!");
  }

  size_t rcol_idx {non_empty[randomize::random_size_t(0, non_empty.size() - 1)]};
  std::swap(keys[lrow_idx][lcol_idx], keys[rrow_idx][rcol_idx]);
}

void Layout::row_exchange() {
  assert(keys.size() > 1);

  size_t max_row_idx {keys.size() - 1};
  size_t lrow_idx {randomize::random_size_t(0, max_row_idx--)};
  size_t rrow_idx {randomize::random_size_t(0, max_row_idx)};

  if (rrow_idx >= lrow_idx) {
    ++rrow_idx;
  }

  size_t limit {std::min(keys[lrow_idx].size(), keys[rrow_idx].size())};
  for (size_t i {0}; i < limit; ++i) {
    if (keys[lrow_idx][i] && keys[rrow_idx][i]) {
      std::swap(keys[lrow_idx][i], keys[rrow_idx][i]);
    }
  }
}

void Layout::row_shuffle() {
  assert(!keys.empty());

  size_t row_idx {randomize::random_size_t(0, keys.size() - 1)};
  std::vector<size_t> non_empty;
  non_empty.reserve(keys.size());

  for (size_t i {0}; i < keys[row_idx].size(); ++i) {
    if (keys[row_idx][i]) {
      non_empty.emplace_back(i);
    }
  }

  size_t max_idx {non_empty.size() - 1};
  for (size_t i {0}; i <= max_idx; ++i) {
    size_t swap_idx {randomize::random_size_t (0, max_idx)};
    std::swap(keys[row_idx][non_empty[i]], keys[row_idx][non_empty[swap_idx]]);
  }
}

void Layout::key_pair_shuffle() {
  assert(!keys.empty());

  size_t max_row_idx {keys.size() - 1};
  size_t lrow_idx {randomize::random_size_t(0, max_row_idx)};
  assert(!keys[lrow_idx].empty());

  std::vector<size_t> non_empty;
  non_empty.reserve(20);

  for (size_t i {0}; i < keys[lrow_idx].size(); ++i) {
    if (keys[lrow_idx][i]) {
      non_empty.emplace_back(i);
    }
  }

  if (non_empty.empty()) {
    throw std::runtime_error(
      "No non-empty cells to pick from for row " +
      std::to_string(lrow_idx) +
      "!"
    );
  }

  size_t lcol_idx {randomize::random_size_t(0, non_empty.size() - 1)};
  non_empty.clear();

  size_t rrow_idx {randomize::random_size_t(0, max_row_idx)};
  for (size_t i {0}; i < keys[rrow_idx].size(); ++i) {
    if (keys[rrow_idx][i]) {
      non_empty.emplace_back(i);
    }
  }

  if (non_empty.empty()) {
    throw std::runtime_error(
      " No non-empty cells to pick from for row " +
      std::to_string(rrow_idx) + 
      "!"
    );
  }

  size_t rcol_idx {randomize::random_size_t(0, non_empty.size() - 1)};
  std::swap(keys[lrow_idx][lcol_idx], keys[rrow_idx][rcol_idx]);
}

int64_t Layout::compute_position_score(double& left_percentage) const {
  int64_t score {0};
  double left_total {0};
  double right_total {0};

  for (size_t i {0}; i < keys.size(); ++i) {
    int64_t row_mult {row_multipliers[i]};
    for (size_t j {0}; j < keys[i].size(); ++j) {
      int64_t col_mult {col_multipliers[j]};
      if (!keys[i][j]) {
        continue;
      }

      int64_t frequency {Ngrams::char_freqs[keys[i][j]]};
      if (j <= last_left) {
        left_total += frequency;
      } else {
        right_total += frequency;
      }

      score += (frequency * row_mult) + (frequency * col_mult);
    }
  }

  left_percentage = (left_total * 100) / (right_total + left_total);
  return score;
}

int64_t Layout::compute_same_finger_score() const {
  int64_t score {0}; 

  size_t row_idx {0}; 
  size_t col_idx {0}; 

  while (col_idx < keys[row_idx].size()) {
    if (!keys[row_idx][col_idx]) {
      ++row_idx;
      continue;
    }

    size_t ngram {keys[row_idx][col_idx]};
    for (size_t i {row_idx + 1}; i < keys.size(); ++i) {
      if (!keys[row_idx][i]) {
        continue;
      }

      ngram = (ngram << sizeof(unsigned char) * CHAR_BIT) + keys[row_idx][i];
      score += Ngrams::bigram_freqs[ngram];

      for (size_t j {i + 1}; j < keys.size(); ++j) {
        if (!keys[row_idx][j]) {
          continue;
        }

        ngram = (ngram << sizeof(unsigned char) * CHAR_BIT) + keys[row_idx][j];
        score += Ngrams::trigram_freqs[ngram];
      }
    }

    assert(row_idx < keys.size());
    if (++row_idx == keys.size()) {
      ++col_idx;
      row_idx = 0;
    }
  }

  return score;
}

int64_t Layout::compute_flow_score() const {
  int64_t score {horizontal_flow_score()};
  score += diagonal_flow_score(0, last_left, 1);
  score += diagonal_flow_score(first_right, keys.size() - 1, -1);
  score += horizontal_zigzag_score();
  score += diagonal_zigzag_score();
  return score;
}

int64_t Layout::horizontal_flow_score() const {
  int64_t score {0};
  std::string ngram;
  ngram.reserve(12);

  for (const auto& row : keys) {
    for (size_t i {0}; i < last_left - 1; ++i) {
      if (row[i].empty()) {
        continue;
      }

      ngram += row[i];
      for (size_t j {i + 1}; j < last_left; ++j) {
        if (row[j].empty()) {
          continue;
        }

        ngram += row[j];
        score += ngram_asymmetries[ngram];

        for (size_t k {j + 1}; k < last_left; ++k) {
          if (row[k].empty()) {
            continue;
          }

          ngram += row[k];
          score += ngram_asymmetries[ngram];
        }
      }
    }

    ngram.clear();
  }
}

Scores Layout::diagonal_flow_score(
  const size_t incl_lower_bound,
  const size_t incl_upper_bound,
  const int sign
) const {
  int64_t pos_score {0};
  int64_t biased_pos_score {0};;
  int64_t diagonal_flow_score {0}; 
  int64_t horizontal_flow_score {0};
  std::string ngram;
  ngram.reserve(12);

  for (size_t r {0}; r < keys.size(); ++r) {
    for (size_t i {incl_lower_bound}; i <= incl_upper_bound - 1; ++i) {
      if (keys[r][i].empty()) {
        continue;
      }

      ngram += keys[r][i];
      pos_score += letter_frequencies[ngram];
      biased_pos_score += 
        (letter_frequencies * row_multipliers[r]) +
        (letter_frequencies * col_multipliers[i]);

      for (size_t s {0}; s < keys.size(); ++s) {
        for (size_t j {i + 1}; j <= incl_upper_bound; ++j) {
          if (keys[s][j].empty()) {
            continue;
          }

          ngram += keys[s][j];
          if (r == s) {
            horizontal_flow_score += ngram_asymmetries[ngram] * sign;
          } else {
            diagonal_flow_score += ngram_asymmetries[ngram] * sign;
          }

          for (size_t t {0}; t < keys.size(); ++t) {
            for (size_t k {j + 1}; k <= incl_upper_bound; ++k) {
              if (keys[t][k].empty()) {
                continue;
              }

              ngram += keys[t][k];
              if (r == s && s == t) {
                horizontal_flow_score += ngram_asymmetries[ngram] * sign;
              } else {
                diagonal_flow_score += ngram_asymmetries[ngram] * sign;
              }
            }
          }
        }
      }

      ngram.clear();
    }
  }

  return Scores {
    pos_score,
    biased_pos_score,
    diagonal_flow_score,
    horizontal_flow_score
  };
}
