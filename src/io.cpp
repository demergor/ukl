#include <cassert>
#include <filesystem>
#include <string>

#include "ukl/io.hpp"
#include "ukl/ngrams.hpp"
#include "ukl/utf8.hpp"

std::filesystem::path resolve(const std::string& path_str) {
  if (path_str.empty() || path_str[0] != '~') { 
    return std::filesystem::path {path_str};
  }

  if (path_str.size() > 1 && path_str[1] != '/' && path_str[1] != '\\') {
    return std::filesystem::path {path_str};
  }

#ifdef _WIN32
  const char* home {std::getenv("USERPROFILE")};
#else 
  const char* home {std::getenv("HOME")};
#endif

  if (!home) {
    return std::filesystem::path(path_str);
  }

  std::filesystem::path home_path {home};
  if (path_str.size() == 1) {
    return home_path;
  }

  return home_path / path_str.substr(2);
}

std::set<std::string> io::fetch_letters(std::ifstream& in) {
  std::set<std::string> letters;
  std::string line;
  size_t count {0};

  while (std::getline(in, line) && count < Ngrams::num_chars) {
    if (line.empty()) {
      continue;
    }

    letters.emplace(line.begin(), line.begin() + utf8::num_bytes(line[0]));
    ++count;
  }

  return letters;
}
