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
#include "postprocessors/ipostprocessor.h"
#include "core/cmd_code.h"

#include "gui/gui_updater.h"

namespace dasm {
namespace core {

class DisassemblerCore {
public:

  ~DisassemblerCore() = default;

  void init(IGUIUpdater* updater_);

  void loadGuessFile(const std::string& fname);

  std::string disassembleInstructionInt(const memory::Addr& addr, size_t& len);

  ///@bug поменять местами возвращаемые значения, чтобы не было такой вырви-глаз конструкции
  size_t disassembleInstruction(const memory::Addr& addr, std::shared_ptr<Chunk>& out_chunk);

  void disassembleBlock(const memory::Addr& addr);

  void setRawMemory(unsigned char* buf, size_t size);

  void initialParse();

  ChunkList& chunks() { return _chunks; }

  memory::Memory& memory() { return _memory; }

  Labels& labels() { return _labels; }

  std::shared_ptr<Label> makeJump(const memory::Addr& from_addr, const memory::Addr& jump_addr, memory::Reference::Type ref_type);

  std::shared_ptr<Label> makeData(const memory::Addr& from_addr, const memory::Addr& data_addr, memory::Reference::Type ref_type);

  LabelPtr addCrossRef(ChunkPtr chunk, const memory::Addr& from_addr, const memory::Addr& dst_addr, memory::Reference::Type ref_type);

  JumpType getLastCmdJumpType(std::shared_ptr<Chunk> chunk, memory::Addr& jump_addr);

  static DisassemblerCore& inst() {
    static DisassemblerCore _inst;
    return _inst;
  }

private:

  DisassemblerCore() {}

  bool labelPresent(const memory::Addr& addr) const;

  size_t postProcessChunk(std::shared_ptr<Chunk> chunk, size_t len);

  ///@brief кого оповещать об обновлении состояния
  IGUIUpdater* updater;
  ///@brief содержимое памяти
  memory::Memory _memory;
  ///@brief распознаные цепочки
  ChunkList _chunks;
  ///@brief метки, собранные в результате дизасма
  Labels _labels;

  std::shared_ptr<postprocessors::AutoCommenter> _auto_commenter;
  std::vector<std::shared_ptr<postprocessors::IPostProcessor>> _postprocessors;
};
}
}

#endif
