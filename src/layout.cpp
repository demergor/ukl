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
  const std::vector<std::pair<size_t, size_t>>& leave_blank,
  const std::vector<std::vector<size_t>>& finger_groupings
) : keys {keyboard::random_layout(width, height, chars, leave_blank)} {
  if (keys.empty()) {
    throw std::runtime_error("keyboard::random_layout returned an empty vector!");
  }

  for (const auto& finger : finger_groupings) {
    std::vector<std::pair<size_t, size_t>> finger_idxs;
    for (const auto& col : finger) {
      for (size_t i {0}; i < keys.size(); ++i) {
        if (keys[i][col]) {
          finger_idxs.emplace_back(i, col);
        }
      }
    }
    finger_pos.emplace_back(finger_idxs);
  }

  if (keys.empty()) {
    throw std::runtime_error("Error: empty layout");
  }

  for (const auto& row : keys) {
    if (row.empty()) {
      throw std::runtime_error("Error: empty row in key layout");
    }
  }

  if (finger_pos.empty()) {
    throw std::runtime_error("Error: no fingers assigned");
  }

  for (const auto& finger : finger_pos) {
    if (finger.empty()) {
      throw std::runtime_error("Error: unassigned finger");
    }
  }
}

void Layout::finger_exchange() {
  assert(finger_pos.size() > 1);

  size_t max_finger_idx {finger_pos.size() - 1};
  size_t lfinger_idx {randomize::random_size_t(0, max_finger_idx--)};
  size_t rfinger_idx {randomize::random_size_t(0, max_finger_idx)};

  if (rfinger_idx >= lfinger_idx) {
    ++rfinger_idx;
  }

  size_t smaller {
    std::min(finger_pos[lfinger_idx].size(), finger_pos[rfinger_idx].size())
  };

  for (size_t i {0}; i < smaller; ++i) {
    const auto& lkey_pos {finger_pos[lfinger_idx][i]};
    const auto& rkey_pos {finger_pos[rfinger_idx][i]};
    std::swap(
       keys[lkey_pos.first][lkey_pos.second],
       keys[rkey_pos.first][rkey_pos.second]
    );
  }
}

void Layout::finger_shuffle() {
  assert(!finger_pos.empty());

  size_t max_finger_idx {finger_pos.size() - 1};
  size_t finger_idx {randomize::random_size_t(0, max_finger_idx)};

  if (finger_pos[finger_idx].size() < 2) {
    return;
  }

  size_t max_same_finger_idx {finger_pos[finger_idx].size() - 1};
  for (size_t i {0}; i <= max_same_finger_idx; ++i) {
    size_t swap_idx {randomize::random_size_t(0, max_same_finger_idx)};
    const auto& lkey_pos {finger_pos[finger_idx][i]};
    const auto& rkey_pos {finger_pos[finger_idx][swap_idx]};
    std::swap(
      keys[lkey_pos.first][lkey_pos.second],
      keys[rkey_pos.first][rkey_pos.second]
    );
  }
}

void Layout::hand_shuffle() {

  assert(finger_pos.size() > 1);
  assert(fp_last_left != fp_first_right);

  const size_t lfinger {randomize::random_size_t(0, fp_last_left)};
  if (finger_pos[lfinger].empty()) {
    throw std::runtime_error("No entries for this finger index: " + std::to_string(lfinger));
  }

  const size_t lfinger_fp {
    randomize::random_size_t(0, finger_pos[lfinger].size() - 1)
  };
  const auto& lkey_pos {finger_pos[lfinger][lfinger_fp]};

  const size_t rfinger {
    randomize::random_size_t(fp_first_right,finger_pos.size() - 1)
  };
  if (finger_pos[rfinger].empty()) {
    throw std::runtime_error("No entries for this finger index: " + std::to_string(rfinger));
  }

  const size_t rfinger_fp {
    randomize::random_size_t(0, finger_pos[rfinger].size() -1)
  };
  const auto& rkey_pos {finger_pos[rfinger][rfinger_fp]};

  std::swap(
    keys[lkey_pos.first][lkey_pos.second],
    keys[rkey_pos.first][rkey_pos.second]
  );
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

void Layout::simple_swap() {
  const size_t lfinger {randomize::random_size_t(0, finger_pos.size() - 1)};
  const size_t rfinger {
    randomize::random_size_t(fp_first_right,finger_pos.size() - 1)
  };

  const size_t lfinger_fp {
    randomize::random_size_t(0, finger_pos[lfinger].size() - 1)
  };
  const size_t rfinger_fp {
    randomize::random_size_t(0, finger_pos[rfinger].size() - 1)
  };

  const auto& lkey_pos {finger_pos[lfinger][lfinger_fp]};
  const auto& rkey_pos {finger_pos[rfinger][rfinger_fp]};

  std::swap(
    keys[lkey_pos.first][lkey_pos.second],
    keys[rkey_pos.first][rkey_pos.second]
  );
}

int64_t Layout::compute_pos_score(double& lpercent) {
  int64_t pos_score {0};
  double lscore {0};
  double rscore {0};

  for (size_t i {0}; i < finger_pos.size(); ++i) {
    for (const auto& key_pos : finger_pos[i]) {
      uint64_t frequency {Ngrams::char_freqs[keys[key_pos.first][key_pos.second]]};
      pos_score += frequency * row_multipliers[key_pos.first];
      pos_score += frequency * col_multipliers[key_pos.second];
        
      if (i <= fp_last_left) {
        lscore += static_cast<double>(frequency);
      } else {
        rscore += static_cast<double>(frequency);
      }
    }
  }

  lpercent = lscore / (rscore + lscore);
  this->pos_score = pos_score;

  return pos_score;
}

int64_t Layout::compute_sf_score() {
  int64_t score {0}; 
  for (const auto& finger : finger_pos) {
    for (size_t i {0}; i < finger.size(); ++i) {
      size_t ngram {keys[finger[i].first][finger[i].second]};
      for (size_t j {i + 1}; j < finger.size(); ++j) {
        size_t bigram {
          (ngram << sizeof(unsigned char) * CHAR_BIT) |
          keys[finger[j].first][finger[j].second]};
        score += Ngrams::bigram_freqs[bigram];

        for (size_t k {j + 1}; k < finger.size(); ++k) {
          size_t trigram {
            (bigram << sizeof(unsigned char) * CHAR_BIT) |
            keys[finger[k].first][finger[k].second]};
          score += Ngrams::trigram_freqs[trigram];
        }
      }
    }
  }

  sf_score = -score;
  return -score;
}

int64_t Layout::compute_flow_score() {
  int64_t score {horizontal_flow_score(0, fp_last_left, 1)};
  score += horizontal_flow_score(fp_first_right, finger_pos.size() - 1, -1);
  score += diagonal_flow_score(0, last_left, 1);
  score += diagonal_flow_score(first_right, finger_pos.size() - 1, -1);

  flow_score = score;
  return score;
}

int64_t Layout::horizontal_flow_score(
  const size_t start_idx,
  const size_t end_idx,
  const int step
) const {
  int64_t score {0};
  for (const auto& row : keys) {
    for (size_t i {start_idx}; i <= end_idx; i += step) {
      if (!row[i]) {
        continue;
      }

      size_t letter {row[i]};
      for (size_t j {i + step}; j <= end_idx; j += step) {
        if (!row[j]) {
          continue;
        }

        size_t bigram {(letter << 8) | row[j]};
        score += Ngrams::bigram_asyms[bigram];

        for (size_t k {j + step}; k <= end_idx; k += step) {
          if (!row[k]) {
            continue;
          }

          size_t trigram {(bigram << 8) | row[k]};
          score += Ngrams::trigram_asyms[trigram];
        }
      }
    }
  }

  return score;
}

int64_t Layout::diagonal_flow_score(
  const size_t first,
  const size_t last,
  const int step
) const {
  int64_t score {0};
  for (size_t i {first}; i < last; i += step) {
    for (const auto letter_pos : finger_pos[i]) {
      size_t letter {keys[letter_pos.first][letter_pos.second]};
      const auto& bi_finger {finger_pos[i + step]};

      for (const auto& bi_pos : bi_finger) {
        if (bi_pos.first == letter_pos.first) {
          continue;
        }

        size_t bigram {
          (letter << sizeof(unsigned char) * CHAR_BIT) |
          keys[bi_pos.first][bi_pos.second]
        };
        score += Ngrams::bigram_asyms[bigram];

        if (i + step == last) {
          continue;
        }

        const auto& tri_finger {finger_pos[2 * step + i]};
        for (const auto& tri_pos : tri_finger) {
          if (tri_pos.first == bi_pos.first) {
            continue;
          }

          size_t trigram {
            (bigram << sizeof(unsigned char) * CHAR_BIT) |
            keys[tri_pos.first][tri_pos.second]
          };
          score += Ngrams::trigram_asyms[trigram];
        }
      }
    }
  }

  return score;
}
