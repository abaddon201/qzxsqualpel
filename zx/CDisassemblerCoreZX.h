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

class DisassemblerCoreZX : public IDisassemblerCore {
public:
  DisassemblerCoreZX(IGUIUpdater* updater);

  void init() override;
  void loadGuessFile(const std::string &fname) override;
  ///@bug поменять местами возвращаемые значения, чтобы не было такой вырви-глаз конструкции
  virtual int disassembleInstruction(const Addr &addr, std::shared_ptr<Chunk>& out_chunk) override;
  void disassembleBlock(const Addr &st_addr) override;
  void initialParse() override;


  Labels &labels() override { return _labels;}
  bool labelPresent(const Addr &addr) const;

  std::shared_ptr<Chunk> createChunk(const Addr &addr, Chunk::Type type=Chunk::Type::UNKNOWN) override;
  ChunkList &chunks() override {return _chunks;}
  bool isChunkEmpty(Chunk* chunk);

  std::shared_ptr<Label> makeJump(const Addr &from_addr, const Addr &jump_addr, Reference::Type ref_type) override;
  virtual Type getLastCmdJumpType(std::shared_ptr<Chunk> chunk, Addr &jump_addr) override;

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
  void parseCommand(std::string &src, Command &out_command);
  void autoCommentCommand(Command &out_command);
  std::string getRST28AutoComment(unsigned char b, int &args_count);
  std::shared_ptr<Label> findKnownLabel(const Addr &addr);

  int postProcessChunk(std::shared_ptr<Chunk> chunk, int len);

  ChunkList _chunks;
  Labels _labels;
  Labels _known_labels;
};

#endif
