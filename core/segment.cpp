#include "segment.h"


void Segment::fill(unsigned char buff[], unsigned long long size) {
  _mem.clear();
  for (unsigned long long i = 0;i<size;++i)
    _mem.push_back(buff[i]);
/*  auto tmp = std::vector<CByte>(buff, buff + size);
  std::copy(tmp.begin(), tmp.end(), _mem.begin());*/
//  _mem = std::vector<unsigned char>(buff, buff + size);
  _dataSize = size;
}
