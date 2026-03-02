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
  inline static constexpr size_t fp_last_left {3};
  inline static constexpr size_t fp_first_right {7};

  std::vector<std::vector<unsigned char>> keys;
  std::vector<std::vector<std::pair<size_t, size_t>>> finger_pos;

  double score;
  int64_t pos_score, sf_score, flow_score;

  Layout(
    const size_t, const size_t,
    std::vector<unsigned char>,
    const std::vector<std::pair<size_t, size_t>>&,
    const std::vector<std::vector<size_t>>&
  );

  Layout(Layout&&) noexcept = default;
  Layout(const Layout&) = default;

  void finger_exchange();
  void finger_shuffle();
  void hand_shuffle();
  void row_exchange();
  void row_shuffle();
  void simple_swap();

  int64_t compute_pos_score(double&);
  int64_t compute_sf_score();
  int64_t compute_flow_score();

  Layout& operator=(const Layout&) = default;

private:
  inline static const int row_multipliers[5] {-8, 1, 10, -5, -10};
  inline static const int col_multipliers[14] {
    -10, -5, -2, 5, 1, -6, -20, -20, -6, 1, 5, -2, -5, -10
  };

  int64_t horizontal_flow_score(const size_t, const size_t, const int) const;
  int64_t diagonal_flow_score(
    const size_t first_finger,
    const size_t last_finger,
    const int step
  ) const;
  int64_t horizontal_zigzag_score() const;
  int64_t diagonal_zigzag_score() const;
};

#endif
