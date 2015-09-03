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

  virtual int disassembleInstruction(const CAddr& addr) override;
  void disassembleBlock(const CAddr& st_addr) override;
  void initialParse() override;


  CLabels& labels() override { return m_Labels;}
  bool labelPresent(CAddr addr) const;

  std::shared_ptr<CChunk> createChunk(const CAddr& addr, CChunk::Type type=CChunk::Type::UNKNOWN) override;
  CChunkList &chunks() override {return m_Chunks;}
  bool isChunkEmpty(CChunk* chunk);

  void makeJump(const CAddr& from_addr, const CAddr& jump_addr, CReference::Type ref_type) override;
  virtual Type getLastCmdJumpType(std::shared_ptr<CChunk> chunk, CAddr &jump_addr) override;

  void setRawMemory(unsigned char* buf, size_t size) override;

private:
  void parseCommand(std::string& src, CCommand& out_command);
  std::string getRST28AutoComment(unsigned char b);
  int postProcessChunk(std::shared_ptr<CChunk> chunk, int len);

  CChunkList m_Chunks;
  CLabels m_Labels;
};

#endif
