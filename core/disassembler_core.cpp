#include "disassembler_core.h"
#include "disassemble.h"
#include "postprocessors/rst28.h"
#include "postprocessors/register_reference_scan.h"

#include "utils/plog/Log.h"

#include <memory>
#include <fstream>

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return (unsigned char)dasm::core::DisassemblerCore::inst().memory().byte(addr);
}

namespace dasm {
namespace core {

void DisassemblerCore::init(IGUIUpdater* updater_) {
  _is_modified = false;
  updater = updater_;

  //_memory = std::make_unique<memory::Memory>();
  ///@bug zx 128 have multiple segments 8k size each
  _memory.createSegment(0, 0xFFFF);

  _auto_commenter = std::make_shared<postprocessors::AutoCommenter>();
  _postprocessors.push_back(std::make_shared<postprocessors::Rst28>());
  _postprocessors.push_back(std::make_shared<postprocessors::RegisterReferenceScan>());
};

void DisassemblerCore::loadGuessFile(const std::string& fname) {
  _is_modified = true;
  _auto_commenter->loadGuessFile(fname);
}

bool DisassemblerCore::labelPresent(uint16_t addr) const {
  return _labels.find(addr) != _labels.end();
}

void DisassemblerCore::setRawMemory(unsigned char* buf, size_t size) {
  _is_modified = true;
  _memory.segment(0)->fill(buf, size);
}

void DisassemblerCore::initialParse() {
  _is_modified = true;
  _commands_map.clear();
  _commands_map.reset(_memory.wholeSize());
  for (unsigned long long i = 0; i < _memory.wholeSize(); ++i) {
    CommandPtr cmd = std::make_shared<Command>();
    auto byte = _memory.byte(i);
    //cmd.command = "db";
    cmd->command_code = CmdCode::NONE;
    cmd->addr = i;
    cmd->len = 1;
    cmd->setUnknownByteArg(byte);
    _commands_map.put(i, 1, cmd);
  }
  updater->updateWidgets();
}

void DisassemblerCore::setEntryPoint(CommandPtr cmd) {
  if (_entry_point != std::numeric_limits<uint32_t>::max()) {
    PLOGD << "Entry point is exists";
    return;
  }
  if (cmd->command_code != CmdCode::NONE && cmd->command_code != CmdCode::DB && cmd->command_code != CmdCode::DW) {
    _is_modified = true;
    _entry_point = cmd->addr;
    bool modified = false;
    if (cmd->blockComment().empty()) {
      cmd->setBlockComment("Entry point");
      modified = true;
    }
    if (cmd->label() == nullptr) {
      auto lbl = std::make_shared<core::Label>(cmd->addr, "ENTRY_POINT");
      cmd->setLabel(lbl);
      modified = true;
    }
    if (modified) {
      updater->onAddressUpdated(cmd->addr, cmd->len);
    }
  }
}

size_t DisassemblerCore::disassembleInstruction(uint16_t addr, CommandPtr& out_cmd) {
  size_t len = 0;
  if ((uint32_t)addr >= _memory.maxAddr()) {
    PLOGE << "address out of range" << utils::toHex(addr);
    return 0;
  }
  std::string buff = disassembleInstructionInt(addr, len);

  if (len) {
    if ((uint32_t)(addr + len) >= _memory.maxAddr()) {
      PLOGD << "instruction out of mem block" << std::endl;
      return -3;
    }
    //check address can be disassembled
    CommandPtr cmd_i;
    if (!_commands_map.get_if(addr, cmd_i)) {
      // address is not loaded
      PLOGD << "no instruction here: " << utils::toHex(addr) << std::endl;
      return 0;
    }
    if ((cmd_i->command_code != CmdCode::NONE) || (cmd_i->addr != addr)) {
      // address allready disassembled
      PLOGD << "allready parsed: " << utils::toHex(addr) << std::endl;
      return 0;
    }
    if (len > 1) {
      // check that all bytes for the command are not parsed, if so, remove their chunks, if not, don't do anything
      for (size_t i = 1; i < len; i++) {
        CommandPtr ch;
        if ((!_commands_map.get_if(addr + i, ch)) || (ch->command_code != CmdCode::NONE)) {
          PLOGD << "Instrunction longer than unparsed block" << std::endl;
          return -4;
        }
      }
    }
    // not at start of memory, possibly we have some code chunk before this address, check it
    LabelPtr klbl{ nullptr };
    if (_auto_commenter) {
      klbl = _auto_commenter->getLabelForAddr(addr);
    }
    PLOGD << "addr=" << utils::toHex(addr) << "command=" << buff << "len=" << len << std::endl;

    CommandPtr cmd = std::make_shared<Command>();
    cmd->addr = addr;
    cmd->len = len;
    cmd->parse(buff);
    if (_auto_commenter) {
      _auto_commenter->commentCommand(cmd);
    }
    if (klbl != nullptr) {
      cmd->setLabel(klbl);
    }
    _commands_map.put(addr, len, cmd);
    PLOGD << "cmd appended" << std::endl;
    len = postProcessCmd(cmd, len);
    out_cmd = cmd;
  }
  return len;
}

void DisassemblerCore::disassembleBlock(uint16_t st_addr) {
  _is_modified = true;
  int res = 0;
  uint16_t addr = st_addr;
  uint16_t cmd_addr;
  CommandPtr last_cmd;
  PLOGD << "disassembleBlock(): addr" << utils::toHex(addr) << std::endl;
  do {
    cmd_addr = addr;
    res = disassembleInstruction(addr, last_cmd);
    if (res == 0) {
      //end of block
      break;
    }
    if (res < 0) {
      //parse error
      return;
    }
    updater->onAddressUpdated(cmd_addr, last_cmd->len);
    uint16_t jump_addr;
    switch (last_cmd->jumpType(jump_addr)) {
      case JumpType::JT_CALL: {
        //call
        PLOGD << "!!!! call: addr=" << utils::toHex(addr) << " to_addr " << utils::toHex(jump_addr) << std::endl;
        PLOGD << "st_addr=" << utils::toHex(st_addr) << std::endl;
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::CALL);
        last_cmd->setJmpAddr(lbl);
        updater->onAddressUpdated(cmd_addr, last_cmd->len);
        addr += res;
        break;
      }
      case JumpType::JT_COND_JUMP: {
        //conditional jump
        PLOGD << "!!!! cond jump: addr=" << utils::toHex(addr) << " to_addr " << utils::toHex(jump_addr) << std::endl;
        //auto& lastcmd = chunk->lastCommand();
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::JUMP);
        last_cmd->setJmpAddr(lbl);
        updater->onAddressUpdated(cmd_addr, last_cmd->len);
        addr += res;
        break;
      }
      case JumpType::JT_JUMP: {
        PLOGD << "!!!! jump: addr=" << utils::toHex(addr) << " to_addr " << utils::toHex(jump_addr) << std::endl;
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::JUMP);
        last_cmd->setJmpAddr(lbl);
        updater->onAddressUpdated(cmd_addr, last_cmd->len);
        res = 0;
        break;
      }
      case JumpType::JT_COND_RET:
        //conditional ret
        PLOGD << "!!!! cond_ret: addr=" << utils::toHex(addr) << std::endl;
        addr += res;
        break;
      case JumpType::JT_RET:
        PLOGD << "!!!! ret: addr=" << utils::toHex(addr) << std::endl;
        res = 0;
        break;
      case JumpType::JT_NONE:
        addr += res;
        break;
    }
  } while (res);
  PLOGD << "finished chunk:st_addr=" << utils::toHex(st_addr) << std::endl;
}

void DisassemblerCore::uncodeBlock(uint16_t addr) {
  CommandPtr cmd;
  if (!_commands_map.get_if(addr, cmd) || cmd->command_code == CmdCode::NONE) {
    return;
  }
  _is_modified = true;

  addr = cmd->addr;
  for (int i = 0; i < cmd->len; ++i, ++addr) {
    auto nc = std::make_shared<Command>();
    nc->addr = addr;
    nc->len = 1;
    nc->command_code = CmdCode::NONE;
    nc->setUnknownByteArg(_memory.byte(addr));
    _commands_map.put(addr, 1, nc);
  }
  updater->onAddressUpdated(cmd->addr, cmd->len);
}

LabelPtr DisassemblerCore::makeJump(uint16_t from_addr, uint16_t jump_addr, memory::Reference::Type ref_type) {
  disassembleBlock(jump_addr);
  CommandPtr jmp_cmd;
  if (!_commands_map.get_if(jump_addr, jmp_cmd)) {
    return nullptr;
  }
  return addCrossRef(jmp_cmd, from_addr, jump_addr, ref_type);
}

LabelPtr DisassemblerCore::addCrossRef(CommandPtr cmd, uint16_t from_addr, uint16_t dst_addr, memory::Reference::Type ref_type) {
  LabelPtr lbl{ nullptr };
  if (_auto_commenter) {
    lbl = _auto_commenter->getLabelForAddr(dst_addr);
  }
  if (cmd->label() == nullptr) {
    lbl = cmd->setLabel(lbl, ref_type);
  } else {
    lbl = cmd->label();
  }
  lbl->addCrossRef(from_addr, ref_type);
  if (!labelPresent(dst_addr)) {
    _labels[dst_addr] = lbl;
  }
  updater->onAddressUpdated(dst_addr, cmd->len);
  return lbl;
}

LabelPtr DisassemblerCore::makeData(uint16_t from_addr, uint16_t data_addr, memory::Reference::Type ref_type) {
  CommandPtr data_cmd;
  if (!_commands_map.get_if(data_addr, data_cmd)) {
    PLOGD << "[makeData] Can't find cmd at: " + utils::toHex(data_addr) << std::endl;
    return nullptr;
  }
  if ((data_cmd->command_code == CmdCode::DB) || (data_cmd->command_code == CmdCode::DW)) {
    //already data
    addCrossRef(data_cmd, from_addr, data_addr, ref_type);
    return data_cmd->label();
  }
  _is_modified = true;
  if ((ref_type == memory::Reference::Type::WRITE_BYTE) || (ref_type == memory::Reference::Type::READ_BYTE)) {
    if (data_cmd->command_code == CmdCode::NONE) {
      auto byte = _memory.byte(data_addr);
      CommandPtr cmd = std::make_shared<Command>();
      cmd->command_code = CmdCode::DB;
      cmd->addr = data_addr;
      cmd->len = 1;
      cmd->setArg(0, std::make_shared<ArgDefault>(byte));
      _commands_map.put(data_addr, 1, cmd);
      updater->onAddressUpdated(data_addr, 1);
    }
  } else if ((ref_type == memory::Reference::Type::WRITE_WORD) || (ref_type == memory::Reference::Type::READ_WORD)) {
    if (data_cmd->command_code == CmdCode::NONE) {
      auto bl = _memory.byte(data_addr);
      auto bh = _memory.byte(data_addr + 1);
      CommandPtr cmd = std::make_shared<Command>();
      cmd->command_code = CmdCode::DW;
      cmd->addr = data_addr;
      cmd->len = 2;
      cmd->setArg(0, std::make_shared<ArgDefault>((((uint16_t)((uint8_t)bh)) << 8) | ((uint16_t)((uint8_t)bl)), ArgSize::Word, true));
      _commands_map.put(data_addr, 2, cmd);
      updater->onAddressUpdated(data_addr, 2);
    }
  }
  if (_commands_map.get_if(data_addr, data_cmd)) {
    return addCrossRef(data_cmd, from_addr, data_addr, ref_type);
  } else {
    return nullptr;
  }
}

void DisassemblerCore::makeArray(uint16_t from_addr, int size, bool clearMem) {
  //FIXME: need to check preconditions (all bytes must be unparsed)
  _is_modified = true;
  if (clearMem) {
    auto addr = from_addr;
    int sz = size;
    for (; sz != 0; sz--, ++addr) {
      _memory.setByte(addr, 0);
    }
  }
  auto addr = from_addr;
  CommandPtr cmd = std::make_shared<Command>();
  cmd->command_code = CmdCode::DB;
  cmd->addr = from_addr;
  cmd->len = size;
  auto arg = std::make_shared<ArgByteArray>(size);
  for (; size != 0; size--, ++addr) {
    auto byte = _memory.byte(addr);
    arg->pushByte(byte);
  }
  cmd->setArg(0, arg);
  _commands_map.put(from_addr, cmd->len, cmd);
  updater->onAddressUpdated(from_addr, cmd->len);
}

std::string DisassemblerCore::disassembleInstructionInt(uint16_t addr, size_t& len) {
  char tbuff[128];
  debugger_disassemble(tbuff, 128, &len, (libspectrum_word)addr);
  return std::string(tbuff);
}

size_t DisassemblerCore::postProcessCmd(CommandPtr cmd, size_t len) {
  for (auto& p : _postprocessors) {
    if (p->checkPrecondition(cmd)) {
      len = p->process(cmd, len);
    }
  }
  return len;
}

bool DisassemblerCore::extractAddrFromRef(const std::string& ref, uint16_t& addr_out) {
  //label
  //sub_0008: //ignore
  //(label)
  //(word_5c5d)
  //(word_5c5f),
  //jmp_1bd1+27/c
  //sub_0c3b+2/c
  //0abc/c
  //jmp_0c86/c
  //jmp_0053
  //ffff
  auto lbl = _labels.getByName(ref);
  if (lbl != nullptr) {
    addr_out = lbl->addr;
    return true;
  } else if (ref.find('(') != ref.npos) {
    //data ref
    std::string refnc = ref;
    refnc.erase(std::remove(refnc.begin(), refnc.end(), '('), refnc.end());
    refnc.erase(std::remove(refnc.begin(), refnc.end(), ')'), refnc.end());
    refnc.erase(std::remove(refnc.begin(), refnc.end(), ','), refnc.end());
    auto lbl = _labels.getByName(refnc);
    if (lbl != nullptr) {
      addr_out = lbl->addr;
      return true;
    }
    auto splited = utils::split(refnc, '_');
    if (splited.size() != 2) {
      PLOGE << "unable to process reference : " << ref << std::endl;
      return false;
      //throw std::runtime_error("unable to process reference: " + ref);
    }
    addr_out = utils::fromHex(splited[1]);
    return true;
  }
  if (ref.find('+') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '/');
    auto spl2 = utils::split(spl1[0], '_');
    auto spl3 = utils::split(spl2[1], '+');
    uint16_t addr = utils::fromHex(spl3[0]);
    auto offs = std::atoi(spl3[1].c_str());
    addr_out = addr + offs;
    return true;
  }
  if (ref.find('/') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '/');
    auto spl2 = utils::split(spl1[0], '_');
    if (spl2.size() == 2) {
      //simple address
      addr_out = utils::fromHex(spl2[1]);
      return true;
    } else {
      //simple address
      addr_out = utils::fromHex(spl2[0]);
      return true;
    }
  }
  if (ref.find('_') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '_');
    addr_out = utils::fromHex(spl1[1]);
    return true;
  }
  if (ref.size() == 4) {
    //value
    try {
      addr_out = utils::fromHex(ref);
      return true;
    } catch (...) {
      PLOGE << "unable to process reference : " << ref << std::endl;
      return false;
    }
  }
  return false;
}
}
}
