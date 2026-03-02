#ifndef NGRAMS_HPP
#define NGRAMS_HPP

#include <array>
#include <cstddef>
#include <cstdint>

struct Ngrams {
public:
  inline static constexpr size_t num_chars {64};
  inline static constexpr size_t byte_states {256};

  inline static std::array<uint64_t, byte_states + 1> char_freqs {};
  inline static std::array<uint64_t, byte_states * byte_states> 
  bigram_freqs {},
  skipgram_freqs {},
  spacegram_freqs {};
  inline static std::array<int64_t, byte_states * byte_states * byte_states> trigram_freqs {};

  inline static std::array<int64_t, byte_states * byte_states> 
  bigram_asyms {},
  skipgram_asyms {},
  spacegram_asyms {};
  inline static std::array<int64_t, byte_states * byte_states * byte_states> trigram_asyms {};
};

#endif
