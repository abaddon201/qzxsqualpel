//
// C++ Interface: IDisassemblerCore
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef IDISASSEMBLERCORE_H
#define IDISASSEMBLERCORE_H

#include "CChunk.h"
#include "CAddr.h"

class IDisassemblerCore {
public:
  typedef enum {
    JT_NONE=0,
    JT_CALL,
    JT_COND_JUMP,
    JT_JUMP,
    JT_COND_RET,
    JT_RET,
  } JumpType;

  virtual ~IDisassemblerCore() {}
  virtual JumpType getLastCmdJumpType(CChunk* chunk, CAddr &jump_addr)=0;
  virtual int disassembleInstruction(QString* res_str, CAddr addr)=0;

};

#endif
