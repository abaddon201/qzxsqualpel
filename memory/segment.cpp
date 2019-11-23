#include "segment.h"

namespace dasm {
namespace memory {

void Segment::fill(unsigned char buff[], size_type size) {
  _mem = std::vector<core::Byte>(buff, buff + size);
  _dataSize = size;
}

}
}
