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

std::shared_ptr<CChunk> CChunkList::createChunk(const CAddr &addr, CChunk::Type type) {
  auto ch = m_Chunks[addr.offset()];
  if (ch!=0) {
    if (ch->type()!=CChunk::Type::UNPARSED)
      return nullptr;
  }
  ch = std::make_shared<CChunk>(addr, type);
  m_Chunks[addr.offset()]=ch;
  return ch;
}

std::shared_ptr<CChunk> CChunkList::getChunkContains(const CAddr &addr) const {
  ///@todo: Переместить арифметику в чанк, чтобы считалась один раз, при создании.
  /// при мелком дизассембле опреатор + вызывается 15 млн. раз... и несёт максимальную нагрузку
  auto res = std::find_if(m_Chunks.begin(), m_Chunks.end(), [&addr](auto& p) {return p.second && (p.second->addr()<=addr) && (p.second->addr()+p.second->length()>addr); });
  if (res == m_Chunks.end())
    return nullptr;
  else
    return res->second;
}

void CChunkList::removeChunk(const CAddr &addr) {
  auto it = find_if(m_Chunks.begin(), m_Chunks.end(), [addr](auto& ptr) {return ptr.second && (ptr.second->addr()==addr);});
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
void CChunkList::printDebug() {
  for (auto ch: m_Chunks) {
    std::cout<<"chunk orig addr:"<<ch.first.toString()<<"::";
    if (ch.second)
      std::cout<<"chunk addr:"<<ch.second->addr().toString()<<std::endl;
    else
      std::cout<<"ERROR: chunk is NULL"<<std::endl;
  }
}
