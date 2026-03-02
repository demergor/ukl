#include <set>
#include <string>
#include <unordered_map>

#include "ukl/conversion.hpp"

std::unordered_map<std::string, unsigned char> conversion::encode(
  const std::set<std::string>& letters
) {
  std::unordered_map<std::string, unsigned char> mapped;
  unsigned char cur {0};
  mapped[" "] = cur++;

  for (const auto& letter : letters) {
    mapped[letter] = cur++;
  }

  return mapped;
}

std::unordered_map<unsigned char, std::string> conversion::rev_map(
  const std::unordered_map<std::string, unsigned char>& map
) {
  std::unordered_map<unsigned char, std::string> result;
  for (const auto& [key, value] : map) {
    result[value] = key;
  }

  return result;
}
