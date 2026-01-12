#include <array>
#include <cstddef>
#include <string>

#include "ukl/ngrams"

template <
  typename RowCount,
  typename PinkyMultitude,
  typename IndexMultitude,
  typename ThumbMultitude
>
struct Layout {
  // ?
};

int main() {
  Ngrams ngrams {io::ngrams::fetch()};
  /*
   * Calculate significance of homerow letters beforehand
   * What's the relation between sum(homerow) and sum(other_keys)?
   */
  keyboard::homerow::calculate(ngrams);
  keyboard::layout::calculate(ngrams, keyboard::homerow);
}
