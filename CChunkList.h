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

#include "CChunk.h"

/**
 * @todo split address space to segment based model. ie change any indices to form like m_Segments[addr.segemnt()].m_Offsets[addr.offset()]
 */
class CChunkList {
public:
    CChunkList();
    CChunk* createChunk(CAddr addr, CChunk::ChunkType type=CChunk::UNKNOWN);
    CChunk* getChunk(const CAddr addr) const;
    CChunk* getChunkContains(CAddr addr) const;
    CChunk* getChunkByPosition(int pos) const;
    void removeChunk(const CAddr addr);
    /// @bug must be setted from memory image size
    CAddr getMaxAddr() const {
        return CAddr(0x0FFFF);
    }
    int count() const;
    void clear();

    void printDebug();
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
        };
        CChunk* operator*() {
            return m_Base->getChunk(m_Addr);
        }
        int operator !=(iterator s) {
            if (s.m_End!=m_End) {
                return true;
            }
            return false;
        }
    private:
        CAddr m_Addr;
        bool m_End;
        const CChunkList* m_Base;
    };
    iterator begin() const {
        return iterator(this, false, 0);
    }
    iterator end() const {
        return iterator(this, true);
    }
    CChunk* operator[] (const CAddr idx) const {
        int i=idx.offset()&0xffff;
        if (m_Chunks[i]==0) {
            return 0;
        }
        return m_Chunks[i];
    }
private:
    //currently this realized by fixed size array
    CChunk* m_Chunks[65536];
};

#endif
