#include <set>
#include <string>
#include <unordered_map>

#include "ukl/conversion.hpp"

// Leaves 0 unmapped to use as equivalent to an empty string
std::unordered_map<std::string, unsigned char> conversion::encode(
  const std::set<std::string>& letters
) {
  std::unordered_map<std::string, unsigned char> mapped;
  unsigned char cur {0};

  for (const auto& letter : letters) {
    mapped[letter] = ++cur;
  }

  return mapped;
}
