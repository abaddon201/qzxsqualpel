#ifndef IMEMORY_H
#define IMEMORY_H

#include <map>
#include <algorithm>
#include <memory>

#include "segment.h"
#include "CAddr.h"
#include "CByte.h"

///@brief Класс представляющий содержимое памяти
class Memory {
public:
  Memory();

  void createSegment(Segment::IdType id, unsigned long long size);
  void switchSegment(Segment::IdType src, Segment::IdType dst);
  std::shared_ptr<Segment> getSegment(Segment::IdType id) {return _segments[id];}

  CByte getByte(const CAddr& addr);

  unsigned long long wholeSize();
private:
  std::map<Segment::IdType, std::shared_ptr<Segment>> _segments;
};

#endif // IMEMORY_H
