#include <string>
#include <unordered_map>

struct Ngrams {
  std::unordered_map<std::string, int>
    letter_freqs, first_freqs, last_freqs,
    bi_freqs, tri_freqs, skip_freqs, space_freqs,
    bi_asyms, tri_asyms, skip_asyms, space_asyms;
};
