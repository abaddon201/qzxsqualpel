#include "segment.h"

namespace dasm {
namespace core {

void Segment::fill(unsigned char buff[], size_type size) {
  _mem = std::vector<Byte>(buff, buff + size);
  _dataSize = size;
}

}
}
