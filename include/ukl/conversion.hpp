#ifndef CONVERSION_HPP
#define CONVERSION_HPP

#include <set>
#include <string>
#include <unordered_map>

namespace conversion {

std::unordered_map<std::string, unsigned char> encode(const std::set<std::string>&);
std::unordered_map<unsigned char, std::string> rev_map(
  const std::unordered_map<std::string, unsigned char>&
);

}

#endif
