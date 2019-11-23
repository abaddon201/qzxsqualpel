//
// C++ Interface: IDisassemblerCore
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef IDISASSEMBLERCORE_H
#define IDISASSEMBLERCORE_H

#include <memory>
#include <functional>

#include "core/chunk.h"
#include "core/chunk_list.h"
#include "core/labels.h"
#include "memory/addr.h"
#include "memory/memory.h"
#include "postprocessors/autocommenter.h"
#include "core/jump_type.h"

#include "gui/gui_updater.h"

namespace dasm {
namespace core {

class IDisassemblerCore {
public:

  IDisassemblerCore(IGUIUpdater* updater_, IDisassemblerCore* inst, std::shared_ptr<postprocessors::AutoCommenter> auto_commenter) : updater{ updater_ }, _auto_commenter{ auto_commenter } {
    _inst = inst;
    _memory = std::make_unique<memory::Memory>();
  }

  virtual ~IDisassemblerCore() = default;

  virtual void init() = 0;

  void loadGuessFile(const std::string& fname);

  virtual std::string disassembleInstructionInt(const memory::Addr& addr, size_t& len) = 0;

  ///@bug поменять местами возвращаемые значения, чтобы не было такой вырви-глаз конструкции
  size_t disassembleInstruction(const memory::Addr& addr, std::shared_ptr<Chunk>& out_chunk);

  void disassembleBlock(const memory::Addr& addr);

  void setRawMemory(unsigned char* buf, size_t size);

  void initialParse();

  std::shared_ptr<Chunk> createChunk(const memory::Addr& addr, Chunk::Type type) { return _chunks.createChunk(addr, type); }

  ChunkList& chunks() { return _chunks; }

  Labels& labels() { return _labels; }

  std::shared_ptr<Label> makeJump(const memory::Addr& from_addr, const memory::Addr& jump_addr, memory::Reference::Type ref_type);

  virtual JumpType getLastCmdJumpType(std::shared_ptr<Chunk> chunk, memory::Addr& jump_addr) = 0;

  Byte getMemoryByte(const memory::Addr& addr) const { return _memory->getByte(addr); }

  static IDisassemblerCore* inst() { return _inst; }

protected:

  bool labelPresent(const memory::Addr& addr) const;

  virtual void parseCommand(std::string& src, Command& out_command) = 0;

  virtual size_t postProcessChunk(std::shared_ptr<Chunk> chunk, size_t len) = 0;

  ///@brief кого оповещать об обновлении состояния
  IGUIUpdater* updater;
  ///@brief указатель на инстанс
  static IDisassemblerCore* _inst;
  ///@brief содержимое памяти
  std::unique_ptr<memory::Memory> _memory;
  ///@brief распознаные цепочки
  ChunkList _chunks;
  ///@brief метки, собранные в результате дизасма
  Labels _labels;

  std::shared_ptr<postprocessors::AutoCommenter> _auto_commenter;
};
}
}

#endif
