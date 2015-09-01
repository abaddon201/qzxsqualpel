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

#include <QDebug>
#include "CChunkList.h"

CChunkList::CChunkList() {
  ///@bug должен определяться загруженным куском
  m_Chunks.resize(65536);
}

std::shared_ptr<CChunk> CChunkList::createChunk(const CAddr addr, CChunk::Type type) {
  if (m_Chunks[addr.offset()]!=0) {
    if (m_Chunks[addr.offset()]->type()!=CChunk::Type::UNPARSED)
      return 0;
  }
  m_Chunks[addr.offset()]=std::make_shared<CChunk>(addr, type);
  return m_Chunks[addr.offset()];
}

std::shared_ptr<CChunk> CChunkList::getChunk(const CAddr addr) const {
  auto c=std::find_if(m_Chunks.begin(), m_Chunks.end(), [addr](auto c) {return (c!=nullptr) && (c->addr() == addr);});
  if (c == m_Chunks.end())
    return nullptr;
  return *c;
}

std::shared_ptr<CChunk> CChunkList::getChunkContains(CAddr addr) const {
  if (m_Chunks[addr.offset()]==0) {
    while (addr!=0) {
      --addr;
      if (m_Chunks[addr.offset()]!=0) {
        return m_Chunks[addr.offset()];
      }
    }
    return 0;
  }
  return m_Chunks[addr.offset()];
}

void CChunkList::removeChunk(CAddr addr) {
  std::remove_if(m_Chunks.begin(), m_Chunks.end(), [&addr](auto ptr) {return ptr && (ptr->addr()==addr);});
}

int CChunkList::count() const {
  /*int count=0;
  for (int i=0; i<65536; i++) {
    if (m_Chunks[i]!=0)
      count++;
  }
  return count;*/
  return m_Chunks.size();
}

void CChunkList::clear() {
  m_Chunks.clear();
  ///@bug должен определять из исходного
  m_Chunks.resize(10000);
}

#include "debug_printers.h"
void CChunkList::printDebug() const {
  CAddr addr;
  do {
    std::shared_ptr<CChunk> chunk=m_Chunks[addr.offset()];
    if (chunk) {
      qDebug()<<"chunk addr:"<<chunk->addr().toString()<<"addr:"<<addr.toString();
    }
    ++addr;
  } while (addr<=getMaxAddr());
}
