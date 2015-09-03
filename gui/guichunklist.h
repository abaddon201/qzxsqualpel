#ifndef GUICHUNKLIST_H
#define GUICHUNKLIST_H

#include <memory>
#include <list>

#include "guichunk.h"

class CAddr;
class CChunkList;

class GUIChunkList {
public:
  std::shared_ptr<GUIChunk> getChunkByPosition(int pos) const;
  std::shared_ptr<GUIChunk> getChunkContains(const CAddr& addr) const;

  std::list<std::shared_ptr<GUIChunk>>& chunks() {return _chunks;}

  void update(CChunkList& chunks);

private:
  std::list<std::shared_ptr<GUIChunk>> _chunks;
};

#endif // GUICHUNKLIST_H
