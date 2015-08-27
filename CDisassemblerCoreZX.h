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

#include "IDisassemblerCore.h"

class CDisassemblerCoreZX : public IDisassemblerCore {
public:
  virtual JumpType getLastCmdJumpType(CChunk* chunk, CAddr &jump_addr);
  virtual int disassembleInstruction(QString* res_str, CAddr addr);
};

#endif
