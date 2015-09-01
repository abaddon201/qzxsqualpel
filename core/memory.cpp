#include "memory.h"

Memory::Memory() {
}

void Memory::createSegment(Segment::IdType id, unsigned long long size) {
  Segment s{id, size};
  std::vector<Segment>::iterator right;
  if (!_segments.empty()) {
    right = std::find_if(_segments.begin(), _segments.end(), [id](auto seg) { return seg.id()> id;});
  } else {
    right = _segments.end();
  }
  _segments.insert(right, s);
}

CByte Memory::getByte(CAddr addr) {
  auto seg = std::find_if(_segments.begin(), _segments.end(), [&addr](auto seg) { return seg.id() == addr.segment();});
  if (seg == _segments.end()) {
    return 0; ///@todo Segment not found, but we don't crash here. Think about what to do... may be throw
  }
  return seg->getByte(addr.offset());
}

unsigned long long Memory::wholeSize() {
  unsigned long long res = 0;

  for (auto s: _segments) {
    res+=s.dataSize();
  }
  return res;
}

