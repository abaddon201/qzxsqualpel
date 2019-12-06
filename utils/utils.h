#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <iostream>

namespace dasm {
namespace utils {

extern std::string _hex[256];

inline auto fromHex(const std::string& val) {
  return std::stoul(val, nullptr, 16);
}

inline const std::string& toHex(uint8_t b) {
  return _hex[b];
}

inline const std::string toHex(uint16_t b) {
  return toHex((uint8_t)(b >> 8)) + toHex((uint8_t)b);
}

inline const std::string toHex(uint16_t b, int size) {
  if (size == 2) {
    return toHex((uint8_t)(b >> 8)) + toHex((uint8_t)b);
  } else if (size == 1) {
    return _hex[b & 0xFF];
  } else {
    throw std::runtime_error("wrong hex size: " + std::to_string(size));
  }
}

void initHexes();

inline std::string tolowerStd(std::string s) {
  ///@bug this shit not found
  //std::transform(s.begin(), s.end(), s.begin(), std::tolower);
//  std::for_each(s.begin(), s.end(), [](char& c){ c=::tolwer(c); });
  return s;
}

inline std::vector<std::string> split(const std::string& s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(std::move(item));
  }
  return elems;
}

inline bool contains(const std::string& src, const std::string& f) {
  return src.find(f) != std::string::npos;
}

}
}
#endif
