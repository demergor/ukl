#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

struct Scores {
  int64_t pos_score;
  int64_t biased_pos_score;
  int64_t diagonal_flow_score;
  int64_t horizontal_flow_score;
};

struct Layout {
public:
  inline static constexpr size_t last_left {6};
  inline static constexpr size_t first_right {7};

  std::vector<std::vector<unsigned char>> keys;
  std::vector<std::vector<size_t*>> same_finger_idxs;

  // Partial scores
  int64_t
  position_score,
  same_finger_score,
  flow_score;

  Layout(
    const size_t, const size_t,
    std::vector<unsigned char>,
    const std::vector<std::pair<const size_t, const size_t>>&,
    const std::vector<std::vector<size_t>>&
  );

  void column_exchange();
  void column_shuffle();
  void finger_shuffle();
  void hand_shuffle();
  void row_exchange();
  void row_shuffle();
  void key_pair_shuffle();

  int64_t compute_position_score(double&) const;
  int64_t compute_same_finger_score() const;
  int64_t compute_flow_score() const;

private:
  inline static const int row_multipliers[5] {-8, 1, 10, -5, -10};
  inline static const int col_multipliers[14] {
    -10, -5, -2, 5, 1, -6, -20, -20, -6, 1, 5, -2, -5, -10
  };

  int64_t horizontal_flow_score() const;
  Scores diagonal_flow_score(
    const size_t incl_lower_bound,
    const size_t incl_upper_bound,
    const int sign
  ) const;
  int64_t horizontal_zigzag_score() const;
  int64_t diagonal_zigzag_score() const;
};

#endif
