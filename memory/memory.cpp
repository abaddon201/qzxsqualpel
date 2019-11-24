#include "memory.h"

namespace dasm {
namespace memory {

Memory::Memory() {}

void Memory::createSegment(Segment::IdType id, size_type size) {
  //std::map<Segment::IdType, Segment>::iterator right;
  if (!_segments.empty()) {
    if (std::find_if(_segments.begin(), _segments.end(), [id](auto seg) { return seg.second->id() == id; }) !=
        _segments.end()) {
      ///@bug: segment exists. What we must do?
      return;
    }
  }
  _segments[id] = std::make_shared<Segment>(id, size);
}

core::Byte Memory::getByte(const Addr& addr) const {
  const auto& seg = _segments.find(addr.segment());
  if (seg == _segments.cend()) {
    throw std::runtime_error("wrong segment");
  }
  return seg->second->getByte(addr.offset());
}

Memory::size_type Memory::wholeSize() {
  size_type res = 0;

  for (auto& s : _segments) {
    res += s.second->dataSize();
  }
  return res;
}

}
}
