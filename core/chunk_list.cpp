//
// C++ Implementation: CChunkList
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "chunk_list.h"

ChunkList::ChunkList() {
}

std::shared_ptr<Chunk> ChunkList::createChunk(const Addr &addr, Chunk::Type type) {

  auto ch1 = _chunks.find(addr.offset());
  if (ch1 != _chunks.end()) {
    if (ch1->second->type() != Chunk::Type::UNPARSED)
      return nullptr;
  }

  std::shared_ptr<Chunk> ch;
  ch = std::make_shared<Chunk>(addr, type);
  _chunks[addr.offset()] = ch;
  return ch;
}

std::shared_ptr<Chunk> ChunkList::getChunkContains(const Addr &addr) const {
  ///@todo подумать что с этой х-ней сделать... 70% нагрузки на ней...
  Addr a = addr;
  do {
    List::const_iterator it;
    if ((it = _chunks.find(a)) != _chunks.end()) {
      return it->second;
    }
    --a;
    ///@bug вообще, тут должна быть ловля исключения, а не проверка. ибо аддр может вполне себе переходить по сегментам (не сейчас)s
  } while (a >= 0);
  return nullptr;
}

void ChunkList::removeChunk(const Addr &addr) {
  auto it = _chunks.find(addr);
  if (it != _chunks.end())
    _chunks.erase(it);
}

int ChunkList::count() const {
  return _chunks.size();
}

void ChunkList::clear() {
  _chunks.clear();
}

#include "debug_printers.h"

void ChunkList::printDebug() {
  for (const auto &ch: _chunks) {
    std::cout << "chunk orig addr:" << ch.first.toString() << "::";
    if (ch.second)
      std::cout << "chunk addr:" << ch.second->addr().toString() << std::endl;
    else
      std::cout << "ERROR: chunk is NULL" << std::endl;
  }
}
