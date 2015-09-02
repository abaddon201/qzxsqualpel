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
  CDisassemblerCoreZX(IGUIUpdater* updater);

  virtual int disassembleInstruction(CAddr addr) override;
  void disassembleBlock(CAddr addr) override;
  void initialParse() override;


  CLabels& labels() override { return m_Labels;}
  bool labelPresent(CAddr addr) const;

  std::shared_ptr<CChunk> createChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN) override;
  CChunkList &chunks() override {return m_Chunks;}
  bool isChunkEmpty(CChunk* chunk);

  void makeJump(CAddr from_addr, CAddr jump_addr, CReference::Type ref_type) override;
  virtual Type getLastCmdJumpType(std::shared_ptr<CChunk> chunk, CAddr &jump_addr) override;

  void setRawMemory(unsigned char* buf, size_t size) override;

private:
  void parseCommand(std::string& src, CCommand& out_command);

  CChunkList m_Chunks;
  CLabels m_Labels;
};

#endif
