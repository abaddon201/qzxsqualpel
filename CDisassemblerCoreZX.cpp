//
// C++ Implementation: CDisassemblerCoreZX
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QDebug>

#include "disassemble.h"

#include "CDisassemblerCoreZX.h"

IDisassemblerCore::Type CDisassemblerCoreZX::getLastCmdJumpType(CChunk* chunk, CAddr &jump_addr) {
  CCommand cmd=chunk->lastCommand();
  if (cmd.command.toLower()=="call") {
    jump_addr=cmd.getJmpAddr();
    return Type::JT_CALL;
  }
  if (((cmd.command.toLower()=="jr") || (cmd.command.toLower()=="jp")) && (cmd.arg2.isEmpty())) {
    if (cmd.arg1.startsWith("(") || cmd.arg1.toLower().contains("ix") || cmd.arg1.toLower().contains("iy") || cmd.arg1.toLower().contains("hl")) {
      //jump to (HL) or (IX) or (IY)
      return Type::JT_RET;
    }
    jump_addr=cmd.getJmpAddr();
    return Type::JT_JUMP;
  }
  if (((cmd.command.toLower()=="jr") || (cmd.command.toLower()=="jp")) && (!cmd.arg2.isEmpty())) {
    jump_addr=cmd.getJmpAddr();
    return Type::JT_COND_JUMP;
  }
  if ((cmd.command.toLower()=="ret") && (!cmd.arg1.isEmpty())) {
    return Type::JT_COND_RET;
  }
  if ((cmd.command.toLower()=="ret") && (cmd.arg1.isEmpty())) {
    return Type::JT_RET;
  }
  if (cmd.command.toLower()=="reti") {
    return Type::JT_RET;
  }
  if (cmd.command.toLower()=="retn") {
    return Type::JT_RET;
  }
  return Type::JT_NONE;
}

int CDisassemblerCoreZX::disassembleInstruction(QString* res_str, CAddr addr) {
  char buff[128];
  size_t len;
  debugger_disassemble( buff, 128, &len, addr.offset() );
  *res_str=buff;
  return len;
}
