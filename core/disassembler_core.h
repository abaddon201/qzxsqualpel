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

#include "memory_map.h"
#include "memory/reference.h"
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
  size_t disassembleInstruction(const memory::Addr& addr, CommandPtr& out_cmd);

  void disassembleBlock(const memory::Addr& addr);

  void setRawMemory(unsigned char* buf, size_t size);

  void initialParse();

  const Labels& labels() const { return _labels; }
  Labels& labels() { return _labels; }

  std::shared_ptr<Label> makeJump(const memory::Addr& from_addr, const memory::Addr& jump_addr, memory::Reference::Type ref_type);

  std::shared_ptr<Label> makeData(const memory::Addr& from_addr, const memory::Addr& data_addr, memory::Reference::Type ref_type);

  void makeArray(const memory::Addr& from_addr, int size, bool clearMem);

  LabelPtr addCrossRef(CommandPtr cmd, const memory::Addr& from_addr, const memory::Addr& dst_addr, memory::Reference::Type ref_type);

  //FIXME: move to command
  JumpType lastCmdJumpType(CommandPtr cmd, memory::Addr& jump_addr);

  std::string fileName() const { return _file_name; }
  void setFileName(const std::string& file_name) { _file_name = file_name; }

  const memory::AddrPtr entryPoint() const { return _entry_point; }
  void setEntryPoint(memory::AddrPtr entry_point) { _entry_point = entry_point; }

  std::shared_ptr<postprocessors::AutoCommenter> autoCommenter() const { return _auto_commenter; }
  void setAutoCommenter(std::shared_ptr<postprocessors::AutoCommenter> ac) { _auto_commenter = ac; }

  const memory::Memory& memory() const { return _memory; }
  memory::Memory& memory() { return _memory; }


  using CommandsMap = MemoryMap<CommandPtr>;
  const CommandsMap& commands() const { return _commands_map; }
  CommandsMap& commands() { return _commands_map; }

  bool extractAddrFromRef(const std::string& ref, memory::Addr& add_out);

  static DisassemblerCore& inst() {
    static DisassemblerCore _inst;
    return _inst;
  }

  void clear() {
    _memory.clear();
    _commands_map.clear();
    _labels.clear();
    _entry_point = nullptr;
    _auto_commenter = nullptr;
  }
private:

  DisassemblerCore() {}

  bool labelPresent(const memory::Addr& addr) const;

  size_t postProcessCmd(CommandPtr cmd, size_t len);

  ///@brief кого оповещать об обновлении состояния
  IGUIUpdater* updater;
  ///@brief содержимое памяти
  memory::Memory _memory;
  ///@brief распознаные цепочки
  CommandsMap _commands_map;
  ///@brief метки, собранные в результате дизасма
  Labels _labels;

  std::string _file_name;

  memory::AddrPtr _entry_point;
  std::shared_ptr<postprocessors::AutoCommenter> _auto_commenter;
  std::vector<std::shared_ptr<postprocessors::IPostProcessor>> _postprocessors;
};
}
}

#endif
