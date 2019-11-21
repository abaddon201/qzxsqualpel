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
#include "core/jump_type.h"

#include "gui/gui_updater.h"

class DisassemblerCoreZX : public dasm::core::IDisassemblerCore {
public:
  DisassemblerCoreZX(IGUIUpdater* updater);

  void init() override;
  std::string disassembleInstructionInt(const dasm::core::Addr &addr, size_t& len) override;

  virtual dasm::core::JumpType getLastCmdJumpType(std::shared_ptr<dasm::core::Chunk> chunk, dasm::core::Addr &jump_addr) override;

  int postProcessChunk(std::shared_ptr<dasm::core::Chunk> chunk, int len) override;

private:
  int command2code(const std::string &cmd) const;

  void parseCommand(std::string &src, dasm::core::Command &out_command) override;

  std::string getRST28AutoComment(unsigned char b, int& args_count);
};

#endif
