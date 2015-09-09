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

#include "core/i_disassembler_core.h"

#include "core/chunk_list.h"
#include "core/labels.h"

class DisassemblerCoreZX : public IDisassemblerCore {
public:
  DisassemblerCoreZX(IGUIUpdater* updater);

  void init() override;
  std::string disassembleInstructionInt(const Addr &addr, size_t& len) override;

  virtual Type getLastCmdJumpType(std::shared_ptr<Chunk> chunk, Addr &jump_addr) override;

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
  void autoCommentCommand(Command &out_command);
  std::string getRST28AutoComment(unsigned char b, int &args_count);

  void parseCommand(std::string &src, Command &out_command) override;
  int postProcessChunk(std::shared_ptr<Chunk> chunk, int len) override;
};

#endif
