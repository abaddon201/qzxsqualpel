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
}

std::shared_ptr<CChunk> CChunkList::createChunk(const CAddr addr, CChunk::Type type) {
  auto ch = m_Chunks[addr.offset()];
  if (ch!=0) {
    if (ch->type()!=CChunk::Type::UNPARSED)
      return 0;
  }
  ch = std::make_shared<CChunk>(addr, type);
  m_Chunks[addr.offset()]=ch;
  return ch;
}

std::shared_ptr<CChunk> CChunkList::getChunk(const CAddr addr) const {
  auto c=std::find_if(m_Chunks.begin(), m_Chunks.end(), [addr](auto c) {return (c.second!=nullptr) && (c.second->addr() == addr);});
  if (c == m_Chunks.end())
    return nullptr;
  return c->second;
}

std::shared_ptr<CChunk> CChunkList::getChunkContains(CAddr addr) const {
  auto res = std::find_if(m_Chunks.begin(), m_Chunks.end(), [&addr](auto p) {return (p.second->addr()<=addr) && (p.second->addr()+p.second->length()>addr); });
  if (res == m_Chunks.end())
    return nullptr;
  else
    return res->second;
}

void CChunkList::removeChunk(CAddr addr) {
  auto it = find_if(m_Chunks.begin(), m_Chunks.end(), [addr](auto ptr) {return ptr.second && (ptr.second->addr()==addr);});
  if (it!=m_Chunks.end())
    m_Chunks.erase(it);
  ///@bug нужно разобраться с этой хренью:std::remove_if(m_Chunks.begin(), m_Chunks.end(), [addr](auto ptr) {return false;/*return ptr.second && (ptr.second->addr()==addr);*/});
}

int CChunkList::count() const {
  return m_Chunks.size();
}

void CChunkList::clear() {
  m_Chunks.clear();
}

#include "debug_printers.h"
void CChunkList::printDebug() const {
  CAddr addr;
  do {
    const std::shared_ptr<CChunk> chunk=(*this)[addr];
    if (chunk) {
      qDebug()<<"chunk addr:"<<chunk->addr().toString()<<"addr:"<<addr.toString();
    }
    ++addr;
  } while (addr<=getMaxAddr());
}
