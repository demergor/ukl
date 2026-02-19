#ifndef LAYOUT_SCORES_H
#define LAYOUT_SCORES_H

#include <atomic>
#include <memory>
#include <vector>

#include "layout.hpp"

struct LayoutScores {
public:
  std::unique_ptr<std::atomic<int64_t>[]> highscores;
  std::vector<Layout> layouts;

  explicit LayoutScores(const size_t, const std::vector<Layout>&);
  explicit LayoutScores(const size_t);
};

#endif
