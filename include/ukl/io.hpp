#ifndef IO_HPP
#define IO_HPP

#include <concepts>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>

namespace io {

std::filesystem::path resolve(const std::string&);
std::set<std::string> fetch_letters(std::ifstream& in);
template <std::integral IntType, size_t N>
void fetch_values(
  std::array<IntType, N>& out,
  std::ifstream& in,
  const std::unordered_map<std::string, unsigned char>& mapping,
  const int multiplier
);

}

#endif
