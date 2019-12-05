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

/*
#ifndef CCHUNKLIST_H
#define CCHUNKLIST_H

#include <memory>
#include <map>
#include <utility>

#include "chunk.h"

namespace dasm {
namespace core {

class ChunkList {
public:
  using List = std::map<const memory::Addr, std::shared_ptr<Chunk>>;

  ChunkList();

  std::shared_ptr<Chunk> createChunk(const memory::Addr& addr, Chunk::Type type = Chunk::Type::UNKNOWN);

  std::shared_ptr<Chunk> getChunkContains(const memory::Addr& addr) const;

  void removeChunk(const memory::Addr& addr);

  void addChunk(const memory::Addr& addr, std::shared_ptr<Chunk> chunk) { _chunks[addr] = std::move(chunk); }

  List& chunks() { return _chunks; }

  const List& chunks() const { return _chunks; }

  size_t count() const {
    return _chunks.size();
  }

  void clear();

  void printDebug();

  std::shared_ptr<Chunk> getChunk(const memory::Addr& addr) {
    auto it = _chunks.find(addr);
    if (it == _chunks.end()) { return nullptr; } else return it->second;
  }

private:
  List _chunks;
};

}
}

#endif
*/