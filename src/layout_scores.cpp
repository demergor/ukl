#include <limits>
#include <memory>
#include <vector>

#include "ukl/layout.hpp"
#include "ukl/layout_scores.hpp"


LayoutScores::LayoutScores(
  const size_t highscores_size,
  const std::vector<Layout>& layouts
) : layouts {layouts}
{
  highscores = std::make_unique<std::atomic<int64_t>[]>(highscores_size);
  for (size_t i = 0; i < highscores_size; ++i) {
    highscores[i].store(std::numeric_limits<long long>::lowest());
  }
}

LayoutScores::LayoutScores(const size_t size) {
  highscores = std::make_unique<std::atomic<int64_t>[]>(size);
  layouts = std::vector<Layout>(size);
}
