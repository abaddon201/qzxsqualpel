#ifndef IMEMORY_H
#define IMEMORY_H

#include <vector>
#include <algorithm>

#include "segment.h"
#include "CAddr.h"
#include "CByte.h"

///@brief Класс представляющий содержимое памяти
class Memory {
public:
  Memory();

  void createSegment(Segment::IdType id, unsigned long long size);
  void switchSegment(Segment::IdType src, Segment::IdType dst);
  Segment &getSegment(Segment::IdType id) {return *std::find_if(_segments.begin(), _segments.end(), [id](auto seg) {return seg.id()==id;});}

  CByte getByte(CAddr addr);

  unsigned long long wholeSize();
private:
  std::vector<Segment> _segments;
};

#endif // IMEMORY_H
