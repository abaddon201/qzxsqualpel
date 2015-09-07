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

  void init() override;
  void loadGuessFile(const std::string &fname) override;
  ///@bug поменять местами возвращаемые значения, чтобы не было такой вырви-глаз конструкции
  virtual int disassembleInstruction(const CAddr &addr, std::shared_ptr<CChunk>& out_chunk) override;
  void disassembleBlock(const CAddr &st_addr) override;
  void initialParse() override;


  CLabels &labels() override { return m_Labels;}
  bool labelPresent(const CAddr &addr) const;

  std::shared_ptr<CChunk> createChunk(const CAddr &addr, CChunk::Type type=CChunk::Type::UNKNOWN) override;
  CChunkList &chunks() override {return m_Chunks;}
  bool isChunkEmpty(CChunk* chunk);

  std::shared_ptr<CLabel> makeJump(const CAddr &from_addr, const CAddr &jump_addr, CReference::Type ref_type) override;
  virtual Type getLastCmdJumpType(std::shared_ptr<CChunk> chunk, CAddr &jump_addr) override;

  void setRawMemory(unsigned char* buf, size_t size) override;

  enum {
    CMD_NONE = 0,
    CMD_CALL,
    CMD_RST,
    CMD_RET,
    CMD_RETI,
    CMD_RETN,
    CMD_JP,
    CMD_JR,
  };
private:
  int command2code(const std::string &cmd) const;
  void parseCommand(std::string &src, CCommand &out_command);
  void autoCommentCommand(CCommand &out_command);
  std::string getRST28AutoComment(unsigned char b, int &args_count);
  std::shared_ptr<CLabel> findKnownLabel(const CAddr &addr);

  int postProcessChunk(std::shared_ptr<CChunk> chunk, int len);

  CChunkList m_Chunks;
  CLabels m_Labels;
  CLabels _known_labels;
};

#endif
