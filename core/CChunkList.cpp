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
  memset(m_Chunks, 0, sizeof(m_Chunks));
}

CChunk* CChunkList::createChunk(const CAddr addr, CChunk::Type type) {
  if (m_Chunks[addr.offset()]!=0) {
    return 0;
  }
  m_Chunks[addr.offset()]=new CChunk(addr, type);
  return m_Chunks[addr.offset()];
}

CChunk* CChunkList::getChunk(const CAddr addr) const {
  return m_Chunks[addr.offset()];
}

CChunk* CChunkList::getChunkContains(CAddr addr) const {
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

CChunk* CChunkList::getChunkByPosition(int pos) const {
  CAddr addr;
  do {
    CChunk* chunk=m_Chunks[addr.offset()];
    if (chunk) {
      qDebug()<<"st="<<chunk->cursorStartPosition()<<", end="<<chunk->cursorEndPosition();
      if ((chunk->cursorStartPosition()<pos) && (chunk->cursorEndPosition()>=pos)) {
        return chunk;
      }
    }
    ++addr;
  } while (addr<=getMaxAddr());
  return 0;
}

void CChunkList::removeChunk(CAddr addr) {
  if (m_Chunks[addr.offset()]!=0) {
    delete m_Chunks[addr.offset()];
    m_Chunks[addr.offset()]=0;
  }
}

int CChunkList::count() const {
  int count=0;
  for (int i=0; i<65536; i++) {
    if (m_Chunks[i]!=0)
      count++;
  }
  return count;
}

void CChunkList::clear() {
  for (int i=0; i<65536; i++) {
    if (m_Chunks[i]!=0) {
      delete m_Chunks[i];
      m_Chunks[i]=0;
    }
  }
}

#include "debug_printers.h"
void CChunkList::printDebug() const {
  CAddr addr;
  do {
    CChunk* chunk=m_Chunks[addr.offset()];
    if (chunk) {
      qDebug()<<"chunk addr:"<<chunk->addr().toString()<<"addr:"<<addr.toString();
    }
    ++addr;
  } while (addr<=getMaxAddr());
}
