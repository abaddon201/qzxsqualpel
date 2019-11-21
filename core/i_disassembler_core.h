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
#include "chunk_list.h"
#include "labels.h"
#include "addr.h"
#include "memory.h"
#include "autocommenter.h"
#include "jump_type.h"

#include "gui/gui_updater.h"

namespace dasm {
namespace core {

class IDisassemblerCore {
public:

  IDisassemblerCore(IGUIUpdater* updater_, IDisassemblerCore* inst, std::shared_ptr<AutoCommenter> auto_commenter) : updater{ updater_ }, _auto_commenter{ auto_commenter } {
    _inst = inst;
    _memory = std::make_unique<Memory>();
  }

  virtual ~IDisassemblerCore() = default;

  virtual void init() = 0;

  virtual std::string disassembleInstructionInt(const Addr& addr, size_t& len) = 0;

  ///@bug поменять местами возвращаемые значения, чтобы не было такой вырви-глаз конструкции
  size_t disassembleInstruction(const Addr& addr, std::shared_ptr<Chunk>& out_chunk);

  void disassembleBlock(const Addr& addr);

  void setRawMemory(unsigned char* buf, size_t size);

  void initialParse();

  std::shared_ptr<Chunk> createChunk(const Addr& addr, Chunk::Type type) { return _chunks.createChunk(addr, type); }

  ChunkList& chunks() { return _chunks; }

  void loadGuessFile(const std::string& fname);

  Labels& labels() { return _labels; }

  std::shared_ptr<Label> makeJump(const Addr& from_addr, const Addr& jump_addr, Reference::Type ref_type);

  virtual JumpType getLastCmdJumpType(std::shared_ptr<Chunk> chunk, Addr& jump_addr) = 0;

  Byte getMemoryByte(const Addr& addr) const { return _memory->getByte(addr); }

  static IDisassemblerCore* inst() { return _inst; }

protected:

  bool labelPresent(const Addr& addr) const;

  std::shared_ptr<Label> findKnownLabel(const Addr& addr);

  virtual void parseCommand(std::string& src, Command& out_command) = 0;

  virtual size_t postProcessChunk(std::shared_ptr<Chunk> chunk, size_t len) = 0;

  ///@brief кого оповещать об обновлении состояния
  IGUIUpdater* updater;
  ///@brief указатель на инстанс
  static IDisassemblerCore* _inst;
  ///@brief содержимое памяти
  std::unique_ptr<Memory> _memory;
  ///@brief распознаные цепочки
  ChunkList _chunks;
  ///@brief метки, собранные в результате дизасма
  Labels _labels;
  ///@brief метки, подгруженные из внешнего файла
  Labels _known_labels;

  std::shared_ptr<AutoCommenter> _auto_commenter;
};
}
}

#endif
