//
// C++ Interface: CDisassemblerCoreZX
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CDISASSEMBLERCOREZX_H
#define CDISASSEMBLERCOREZX_H

#include "core/IDisassemblerCore.h"

#include "core/CChunkList.h"
#include "core/CLabels.h"

class CDisassemblerCoreZX : public IDisassemblerCore {
public:
  CDisassemblerCoreZX(IGUIUpdater* updater) : IDisassemblerCore{updater, this} {}

  virtual Type getLastCmdJumpType(CChunk* chunk, CAddr &jump_addr) override;
  virtual int disassembleInstruction(CAddr addr) override;
  void disassembleBlock(CAddr addr) override;

  CChunk* createChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN) override;

  CLabels labels() const override {
    return m_Labels;
  }

  bool labelPresent(CAddr addr) const ;

  CChunkList &chunks() override {
    return m_Chunks;
  }

  void makeJump(CAddr from_addr, CAddr jump_addr, CReference::Type ref_type) override;
  void setRawMemory(unsigned char* buf, size_t size) override;

  bool isChunkEmpty(CChunk* chunk);
  void initialParse() override;

private:
  CChunkList m_Chunks;
  CLabels m_Labels;
  int m_ProgLength;
  CByte m_MemoryPool[0xFFFF];

  CByte getMemoryByte(CAddr addr) const override;
};

#endif
