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
#include "core/registers.h"

#include "gui/gui_updater.h"

namespace dasm {

class DisassemblerCoreZX : public core::IDisassemblerCore {
public:
  DisassemblerCoreZX(IGUIUpdater* updater);

  void init() override;
  std::string disassembleInstructionInt(const memory::Addr& addr, size_t& len) override;

  virtual dasm::core::JumpType getLastCmdJumpType(std::shared_ptr<core::Chunk> chunk, memory::Addr& jump_addr) override;

  size_t postProcessChunk(std::shared_ptr<core::Chunk> chunk, size_t len) override;

private:
  int command2code(const std::string& cmd) const;

  void parseCommand(std::string& src, core::Command& out_command) override;

  dasm::core::Register16 getRegister(const std::string& arg);

  bool isLDICmd(const core::Command& cmd);

  size_t processRST28(std::shared_ptr<core::Chunk> chunk, size_t len, const memory::Addr& addr);

  std::string getRST28AutoComment(unsigned char b, int& args_count);
};

}

#endif
