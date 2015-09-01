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

#include <memory>
#include <functional>

#include "CChunk.h"
#include "CAddr.h"
#include "memory.h"

class CLabels;
class CChunkList;

class IGUIUpdater {
public:
  virtual void updateWidgets() = 0;
  virtual ~IGUIUpdater() {}
};

class IDisassemblerCore {
public:
  enum class Type{
    JT_NONE=0,
    JT_CALL,
    JT_COND_JUMP,
    JT_JUMP,
    JT_COND_RET,
    JT_RET,
  };

  IDisassemblerCore(IGUIUpdater* updater_, IDisassemblerCore* inst) : updater{updater_} {_inst=inst; _memory=std::unique_ptr<Memory> {new Memory()};}
  virtual ~IDisassemblerCore() {}

  virtual int disassembleInstruction(CAddr addr)=0;
  virtual void disassembleBlock(CAddr addr) = 0;
  virtual void setRawMemory(unsigned char* buf, size_t size) = 0;
  virtual void initialParse() = 0;

  virtual std::shared_ptr<CChunk> createChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN) = 0;
  virtual CChunkList &chunks() = 0;

  virtual CLabels& labels() = 0;

  virtual void makeJump(CAddr from_addr, CAddr jump_addr, CReference::Type ref_type) = 0;
  virtual Type getLastCmdJumpType(std::shared_ptr<CChunk> chunk, CAddr &jump_addr)=0;

  CByte getMemoryByte(CAddr addr) const {
    return _memory->getByte(addr);
  }

  static IDisassemblerCore* inst() {return _inst;}

protected:
  IGUIUpdater* updater;
  static IDisassemblerCore* _inst;
  std::unique_ptr<Memory> _memory;
};

#endif
