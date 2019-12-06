#ifndef IMEMORY_H
#define IMEMORY_H

#include <map>
#include <algorithm>
#include <memory>
#include <string>

#include "segment.h"

namespace dasm {
namespace memory {

///@brief Класс представляющий содержимое памяти
class Memory {
public:
  using size_type = size_t;

  Memory();

  void createSegment(Segment::IdType id, size_type size);

  void switchSegment(Segment::IdType src, Segment::IdType dst);

  void placeSegment(SegmentPtr seg) {
    _segments[seg->id()] = seg;
  }

  std::shared_ptr<Segment> segment(Segment::IdType id) const {
    auto seg_it = _segments.find(id);
    if (seg_it != _segments.end()) {
      return seg_it->second;
    }
    throw std::runtime_error("unknown segment: " + std::to_string(id));
  }

  const std::map<Segment::IdType, std::shared_ptr<Segment>>& segments() const { return _segments; }

  uint32_t maxAddr() const { return _segments.rbegin()->second->dataSize(); }

  uint8_t byte(uint16_t addr) const;

  void setByte(uint16_t addr, uint8_t b);

  size_type wholeSize();

  void clear() {
    _segments.clear();
  }

private:
  std::map<Segment::IdType, std::shared_ptr<Segment>> _segments;
};

}
}

#endif // IMEMORY_H
