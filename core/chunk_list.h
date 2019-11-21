//
// C++ Interface: CChunkList
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCHUNKLIST_H
#define CCHUNKLIST_H

#include <memory>
#include <map>
#include <utility>

#include "chunk.h"

namespace dasm {
namespace core {

/**
 * @todo split address space to segment based model. ie change any indices to form like m_Segments[addr.segemnt()].m_Offsets[addr.offset()]
 */
class ChunkList {
public:
  using List = std::map<const Addr, std::shared_ptr<Chunk>>;

  ChunkList();

  std::shared_ptr<Chunk> createChunk(const Addr& addr, Chunk::Type type = Chunk::Type::UNKNOWN);

  std::shared_ptr<Chunk> getChunkContains(const Addr& addr) const;

  void removeChunk(const Addr& addr);

  void addChunk(const Addr& addr, std::shared_ptr<Chunk> chunk) { _chunks[addr] = std::move(chunk); }

  List& chunks() { return _chunks; }

  size_t count() const {
    return _chunks.size();
  }

  void clear();

  void printDebug();

  std::shared_ptr<Chunk> getChunk(const Addr& addr) {
    auto it = _chunks.find(addr);
    if (it == _chunks.end()) { return nullptr; } else return it->second;
  }

private:
  List _chunks;
};

}
}

#endif
