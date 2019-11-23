//
// C++ Implementation: CChunkList
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "chunk_list.h"

#include "debug_printers.h"


namespace dasm {
namespace core {

ChunkList::ChunkList() {}

std::shared_ptr<Chunk> ChunkList::createChunk(const memory::Addr& addr, Chunk::Type type) {

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

std::shared_ptr<Chunk> ChunkList::getChunkContains(const memory::Addr& addr) const {
  ///@todo подумать что с этой х-ней сделать... 70% нагрузки на ней...
  memory::Addr a = addr;
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

void ChunkList::removeChunk(const memory::Addr& addr) {
  auto it = _chunks.find(addr);
  if (it != _chunks.end())
    _chunks.erase(it);
}

void ChunkList::clear() {
  _chunks.clear();
}

void ChunkList::printDebug() {
  for (const auto& ch : _chunks) {
    std::cout << "chunk orig addr:" << ch.first.toString() << "::";
    if (ch.second)
      std::cout << "chunk addr:" << ch.second->addr().toString() << std::endl;
    else
      std::cout << "ERROR: chunk is NULL" << std::endl;
  }
}

}
}
