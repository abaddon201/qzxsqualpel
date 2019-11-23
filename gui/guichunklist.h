#ifndef GUICHUNKLIST_H
#define GUICHUNKLIST_H

#include <memory>
#include <list>

#include "guichunk.h"
#include "memory/addr.h"
#include "core/chunk_list.h"

class GUIChunkList {
public:
  GUIChunkPtr getChunkByPosition(int pos) const;

  GUIChunkPtr getChunkContains(const dasm::memory::Addr &addr) const;

  std::list<GUIChunkPtr> &chunks() { return _chunks; }

  void update(dasm::core::ChunkList &chunks);

private:
  std::list<GUIChunkPtr> _chunks;
};

#endif // GUICHUNKLIST_H
