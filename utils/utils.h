#ifndef CORE_UTILS_H
#define CORE_UTILS_H

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <iostream>

extern std::string _hex[256];

inline const std::string &byte2hex(unsigned char b) {
  return _hex[b];
}

///@todo make from this things class
template<typename T>
std::string hexify(T i, int len = 0) {
  std::stringbuf buf;
  std::ostream os(&buf);
  len = len ? len : (sizeof(T) * 2);

  os << std::setfill('0') << std::setw(len) << std::hex << (unsigned long long) i;
  return buf.str();
}

inline void initHexes() {
  for (int i = 0; i < 256; ++i)
    _hex[i] = hexify(i, 2);
}

inline std::string tolowerStd(std::string s) {
  ///@bug this shit not found
  //std::transform(s.begin(), s.end(), s.begin(), std::tolower);
//  std::for_each(s.begin(), s.end(), [](char& c){ c=::tolwer(c); });
  return s;
}

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(std::move(item));
  }
  return elems;
}

inline bool contains(const std::string &src, const std::string &f) {
  return src.find(f) != std::string::npos;
}

#endif
