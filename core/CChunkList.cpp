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

#include "CChunkList.h"

CChunkList::CChunkList() {
}

std::shared_ptr<CChunk> CChunkList::createChunk(const CAddr &addr, CChunk::Type type) {

  auto ch1 = m_Chunks.find(addr.offset());
  if (ch1 != m_Chunks.end()) {
    if (ch1->second->type()!=CChunk::Type::UNPARSED)
      return nullptr;
  }

  std::shared_ptr<CChunk> ch;
  ch = std::make_shared<CChunk>(addr, type);
  m_Chunks[addr.offset()]=ch;
  return ch;
}

std::shared_ptr<CChunk> CChunkList::getChunkContains(const CAddr &addr) const {
  ///@todo подумать что с этой х-ней сделать... 70% нагрузки на ней...
  CAddr a = addr;
  do {
    List::const_iterator it;
    if ((it = m_Chunks.find(a)) != m_Chunks.end()) {
      return it->second;
    }
    --a;
    ///@bug вообще, тут должна быть ловля исключения, а не проверка. ибо аддр может вполне себе переходить по сегментам (не сейчас)s
  } while (a>=0);
  return nullptr;
}

void CChunkList::removeChunk(const CAddr &addr) {
  auto it = m_Chunks.find(addr);
  if (it!=m_Chunks.end())
    m_Chunks.erase(it);
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
