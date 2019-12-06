#include "utils.h"

namespace dasm {
namespace utils {

std::string _hex[256];

template<typename T>
std::string hexify(T i, int len = 0) {
  std::stringbuf buf;
  std::ostream os(&buf);
  len = len ? len : (sizeof(T) * 2);

  os << std::setfill('0') << std::setw(len) << std::hex << (unsigned long long) i;
  return buf.str();
}

void initHexes() {
  for (int i = 0; i < 256; ++i)
    _hex[i] = hexify(i, 2);
}

}
}
