#include "memory.h"

Memory::Memory() {
}

void Memory::createSegment(Segment::IdType id, unsigned long long size) {
  //std::map<Segment::IdType, Segment>::iterator right;
  if (!_segments.empty()) {
    if (std::find_if(_segments.begin(), _segments.end(), [id](auto seg) { return seg.second->id() == id;})!=_segments.end()) {
      ///@bug: segment exists. What we must do?
      return;
    }
  }
  _segments[id] = std::make_shared<Segment>(id, size);
}

Byte Memory::getByte(const Addr& addr) {
  return _segments[addr.segment()]->getByte(addr.offset());
}

unsigned long long Memory::wholeSize() {
  unsigned long long res = 0;

  for (auto s: _segments) {
    res+=s.second->dataSize();
  }
  return res;
}

