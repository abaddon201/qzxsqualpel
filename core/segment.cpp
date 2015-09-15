#include "segment.h"

void Segment::fill(unsigned char buff[], size_type size) {
  _mem = std::vector<Byte>(buff, buff + size);
  _dataSize = size;
}
