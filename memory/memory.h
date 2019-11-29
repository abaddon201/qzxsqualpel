#ifndef IMEMORY_H
#define IMEMORY_H

#include <map>
#include <algorithm>
#include <memory>

#include "segment.h"
#include "addr.h"
#include "core/byte.h"

namespace dasm {
namespace memory {

///@brief Класс представляющий содержимое памяти
class Memory {
public:
  using size_type = size_t;

  Memory();

  void createSegment(Segment::IdType id, size_type size);

  void switchSegment(Segment::IdType src, Segment::IdType dst);

  std::shared_ptr<Segment> getSegment(Segment::IdType id) { return _segments[id]; }

  Addr getMaxAddr() const { return _segments.rbegin()->second->dataSize(); }

  core::Byte getByte(const Addr& addr) const;

  void setByte(const Addr& addr, core::Byte& b);

  size_type wholeSize();

private:
  std::map<Segment::IdType, std::shared_ptr<Segment>> _segments;
};

}
}

#endif // IMEMORY_H
