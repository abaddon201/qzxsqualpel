#ifndef GUICHUNKLIST_H
#define GUICHUNKLIST_H

#include <memory>
#include <list>

#include "guichunk.h"

class Addr;
class ChunkList;

class GUIChunkList {
public:
  std::shared_ptr<GUIChunk> getChunkByPosition(int pos) const;
  std::shared_ptr<GUIChunk> getChunkContains(const Addr& addr) const;

  std::list<std::shared_ptr<GUIChunk>>& chunks() {return _chunks;}

  void update(ChunkList& chunks);

private:
  std::list<std::shared_ptr<GUIChunk>> _chunks;
};

#endif // GUICHUNKLIST_H
