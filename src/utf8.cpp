#include <cstddef>
#include <stdexcept>

#include "ukl/utf8.hpp"

/**
  * 0xxxxxxx -> ASCII (only 1 byte)
  * 110xxxxx -> start of 2-byte-sequence
  * 1110xxxx -> start of 3-byte-sequence
  * 11110xxx -> start of 4-byte-sequence (max)
  *
  * 10xxxxxx -> continuation byte
  */

size_t utf8::num_bytes(const unsigned char byte) {
  if ((byte & 0b10000000) == 0b00000000) {
    return 1;
  } else if ((byte & 0b11100000) == 0b11000000) {
    return 2;
  } else if ((byte & 0b11110000) == 0b11100000) {
    return 3;
  } else if ((byte & 0b11111000) == 0b11110000) {
    return 4;
  } else {
    throw std::runtime_error("Not a valid start of a byte sequence!");
  }
}
