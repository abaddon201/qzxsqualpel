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
#include <vector>

#include "CChunk.h"

/**
 * @todo split address space to segment based model. ie change any indices to form like m_Segments[addr.segemnt()].m_Offsets[addr.offset()]
 */
class CChunkList {
public:
  using List = std::vector<std::shared_ptr<CChunk>>;

  CChunkList();
  std::shared_ptr<CChunk> createChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN);
  std::shared_ptr<CChunk> getChunk(const CAddr addr) const;
  std::shared_ptr<CChunk> getChunkContains(CAddr addr) const;
  void removeChunk(const CAddr addr);
  List& chunks() {return m_Chunks;}
  /// @bug must be setted from memory image size
  CAddr getMaxAddr() const { return CAddr(0x0FFFF); }
  int count() const;
  void clear();

  void printDebug() const;

  class iterator {
  public:
    iterator(const CChunkList* base, bool end=false, CAddr addr=CAddr()) : m_Addr(addr), m_End(end), m_Base(base) {}
    iterator operator++() {
      while (true) {
        if (m_Addr==m_Base->getMaxAddr()) {
          *this=m_Base->end();
          return m_Base->end();
        }
        ++m_Addr;
        if (m_Base->getChunk(m_Addr)) {
          return *this;
        }
      }
    }
    std::shared_ptr<CChunk> operator*() { return m_Base->getChunk(m_Addr); }
    int operator !=(iterator s) { return (s.m_End!=m_End); }
  private:
    CAddr m_Addr;
    bool m_End;
    const CChunkList* m_Base;
  };

  iterator begin() const { return iterator(this, false, 0); }
  iterator end() const { return iterator(this, true); }
  std::shared_ptr<CChunk> operator[] (const CAddr idx) const {
    int i=idx.offset()&0xffff;
    if (m_Chunks[i]==0) {
      return 0;
    }
    return m_Chunks[i];
  }
private:
  List m_Chunks;
};

#endif
