//
// C++ Interface: CChunkList
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCHUNKLIST_H
#define CCHUNKLIST_H

#include <memory>
#include <map>

#include "CChunk.h"

/**
 * @todo split address space to segment based model. ie change any indices to form like m_Segments[addr.segemnt()].m_Offsets[addr.offset()]
 */
class CChunkList {
public:
  using List = std::map<const CAddr, std::shared_ptr<CChunk>>;

  CChunkList();
  std::shared_ptr<CChunk> createChunk(const CAddr &addr, CChunk::Type type=CChunk::Type::UNKNOWN);
  std::shared_ptr<CChunk> getChunkContains(const CAddr &addr) const;
  void removeChunk(const CAddr &addr);
  void addChunk(const CAddr &addr, std::shared_ptr<CChunk> chunk) { m_Chunks[addr]=chunk;}
  List &chunks() {return m_Chunks;}
  int count() const;
  void clear();

  void printDebug();

  std::shared_ptr<CChunk> operator[] (const CAddr &idx) { return m_Chunks[idx]; }
  std::shared_ptr<CChunk> __attribute__ ((deprecated)) getChunk(const CAddr &addr) { return m_Chunks[addr]; }
private:
  List m_Chunks;
};

#endif
