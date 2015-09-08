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

#include "chunk.h"
#include "addr.h"
#include "memory.h"

class Labels;
class ChunkList;

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

  virtual void init() = 0;

  virtual int disassembleInstruction(const Addr &addr, std::shared_ptr<Chunk>& out_chunk)=0;
  virtual void disassembleBlock(const Addr& addr) = 0;
  virtual void setRawMemory(unsigned char* buf, size_t size) = 0;
  virtual void initialParse() = 0;

  virtual std::shared_ptr<Chunk> createChunk(const Addr& addr, Chunk::Type type=Chunk::Type::UNKNOWN) = 0;
  virtual ChunkList &chunks() = 0;

  virtual Labels& labels() = 0;

  virtual std::shared_ptr<Label> makeJump(const Addr& from_addr, const Addr& jump_addr, Reference::Type ref_type) = 0;
  virtual Type getLastCmdJumpType(std::shared_ptr<Chunk> chunk, Addr &jump_addr)=0;

  Byte getMemoryByte(const Addr& addr) const {return _memory->getByte(addr);}

  virtual void loadGuessFile(const std::string& fname) = 0;

  static IDisassemblerCore* inst() {return _inst;}

protected:
  IGUIUpdater* updater;
  static IDisassemblerCore* _inst;
  std::unique_ptr<Memory> _memory;
};

#endif
