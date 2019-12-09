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

  void disassembleBlock(uint16_t addr);

  void setRawMemory(unsigned char* buf, size_t size);

  void initialParse();

  const Labels& labels() const { return _labels; }
  Labels& labels() { return _labels; }

  LabelPtr makeData(uint16_t from_addr, uint16_t data_addr, memory::Reference::Type ref_type);
  void makeArray(uint16_t from_addr, int size, bool clearMem);

  std::string fileName() const { return _file_name; }
  void setFileName(const std::string& file_name) { _file_name = file_name; }

  const uint16_t entryPoint() const { return _entry_point; }
  bool entryPointExists() const { return _entry_point != std::numeric_limits<uint32_t>::max(); }
  void setEntryPoint(CommandPtr cmd);
  void setEntryPoint(uint32_t ep) { _entry_point = ep; }

  std::shared_ptr<postprocessors::AutoCommenter> autoCommenter() const { return _auto_commenter; }
  void setAutoCommenter(std::shared_ptr<postprocessors::AutoCommenter> ac) { _auto_commenter = ac; }

  const memory::Memory& memory() const { return _memory; }
  memory::Memory& memory() { return _memory; }

  using CommandsMap = MemoryMap<CommandPtr>;
  const CommandsMap& commands() const { return _commands_map; }
  CommandsMap& commands() { return _commands_map; }

  bool extractAddrFromRef(const std::string& ref, uint16_t& add_out);

  bool isModified() const { return _is_modified; }
  void setModified() { _is_modified = true; }
  void resetModified() { _is_modified = false; }

  static DisassemblerCore& inst() {
    static DisassemblerCore _inst;
    return _inst;
  }

  void clear() {
    _memory.clear();
    _commands_map.clear();
    _labels.clear();
    _auto_commenter = nullptr;
  }
private:

  DisassemblerCore() {}

  std::string disassembleInstructionInt(uint16_t addr, size_t& len);

  size_t disassembleInstruction(uint16_t addr, CommandPtr& out_cmd);
  LabelPtr makeJump(uint16_t from_addr, uint16_t jump_addr, memory::Reference::Type ref_type);
  LabelPtr addCrossRef(CommandPtr cmd, uint16_t from_addr, uint16_t dst_addr, memory::Reference::Type ref_type);

  bool labelPresent(uint16_t addr) const;

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

  bool _is_modified;

  uint32_t _entry_point = std::numeric_limits<uint32_t>::max();
  std::shared_ptr<postprocessors::AutoCommenter> _auto_commenter;
  std::vector<std::shared_ptr<postprocessors::IPostProcessor>> _postprocessors;
};
}
}

#endif
