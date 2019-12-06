#include "segment.h"

namespace dasm {
namespace memory {

void Segment::fill(unsigned char buff[], size_type size) {
  _mem = std::vector<uint8_t>(buff, buff + size);
  _dataSize = size;
}

}
}
