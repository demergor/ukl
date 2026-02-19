#ifndef NGRAMS_HPP
#define NGRAMS_HPP

#include <array>
#include <cstddef>
#include <cstdint>

struct Ngrams {
public:
  inline static constexpr size_t num_chars {80};

  inline static  std::array<uint32_t, num_chars> char_freqs {};
  inline static  std::array<uint32_t, num_chars * num_chars> 
  bigram_freqs {},
  skipgram_freqs {},
  spacegram_freqs {};
  inline static std::array<uint32_t, num_chars * num_chars * num_chars> trigram_freqs {};

  inline static std::array<int32_t, num_chars * num_chars> 
  bigram_asyms {},
  skipgram_asyms {},
  spacegram_asyms {};
  inline static std::array<uint32_t, num_chars * num_chars * num_chars> trigram_asyms {};
};

#endif
