//
// C++ Implementation: CCommand
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "command.h"
#include "disassembler_core.h"
#include "utils/utils.h"
#include "utils/plog/Log.h"
#include "flag.h"

namespace dasm {
namespace core {

uint16_t Command::getJmpAddrFromString() const {
  if (_args.size() == 1) {
    //get from arg1
    return argConvert<ArgDefault>(_args[0])->value();
  } else {
    //get from arg2
    return argConvert<ArgDefault>(_args[1])->value();
  }
}

uint16_t Command::getJmpAddr() const {
  if (_args.size() == 1) {
    //get from arg1
    return dynamic_cast<ArgMemoryReference*>(_args[0].get())->label->addr;
  } else {
    //get from arg2
    return dynamic_cast<ArgMemoryReference*>(_args[1].get())->label->addr;
  }
}

void Command::setJmpAddr(const LabelPtr label) {
  // if label is nullptr, we can't change default type arg to the label (we don't know about it)
  if (label != nullptr) {
    if (_args.size() == 1) {
      _args[0] = std::make_shared<ArgMemoryReference>(label, false);
    } else {
      _args[1] = std::make_shared<ArgMemoryReference>(label, false);
    }
  }
}

void Command::addCrossRef(uint16_t addr, memory::Reference::Type type) {
  memory::ReferencePtr ref = std::make_shared<memory::Reference>(addr, type);
  _references.push_back(ref);
}

LabelPtr Command::setLabel(LabelPtr label, memory::Reference::Type ref_type) {
  if (label == nullptr) {
    //generate from name
    std::string t1{ utils::toHex(addr) };
    switch (ref_type) {
      case memory::Reference::Type::JUMP:
        _label = std::make_shared<Label>(addr, std::string("jmp_") + t1);
        break;
      case memory::Reference::Type::CALL:
        _label = std::make_shared<Label>(addr, std::string("sub_") + t1);
        break;
      case memory::Reference::Type::READ_BYTE:
      case memory::Reference::Type::WRITE_BYTE:
        _label = std::make_shared<Label>(addr, std::string("byte_") + t1);
        break;
      case memory::Reference::Type::READ_WORD:
      case memory::Reference::Type::WRITE_WORD:
        _label = std::make_shared<Label>(addr, std::string("word_") + t1);
        break;
    }
  } else {
    _label = label;
  }
  return _label;
}

std::string Command::getArgsString() const {
  if (_args.size() == 0) {
    return std::string();
  }
  if (_args.size() == 1) {
    return _args[0]->toString();
  }
  return _args[0]->toString() + ", " + _args[1]->toString();
}

std::string Command::getOpcodesString(size_t opcodes_count) const {
  std::string tmp;
  auto l1 = len;
  std::string suffix = "";
  if (opcodes_count != -1) {
    l1 = std::min(l1, opcodes_count);
    if (l1 < len) {
      suffix = " ...";
    }
  }
  uint16_t a = addr;
  bool first = true;
  for (; l1; --l1, ++a) {
    if (first) {
      first = false;
    } else {
      tmp += " ";
    }
    tmp += utils::toHex(DisassemblerCore::inst().memory().byte(a));
  }
  tmp += suffix;
  return tmp;
}

uint8_t Command::opcodes(unsigned long long offs) const {
  return DisassemblerCore::inst().memory().byte(addr + offs);
}

void Command::parse(std::string& src) {
  PLOGD << utils::toHex(addr);
  if (addr == 0x03c1) {
    PLOGD << "src: " << src << std::endl;
  }
  PLOGD << "src: " << src << std::endl;
  std::vector<std::string> strlist = utils::split(src, ' ');
  auto command = strlist[0];
  command_code = CmdCode{ command };
  if (strlist.size() > 1) {
    //has args
    std::vector<std::string> args = utils::split(strlist[1], ',');
    for (const auto& a : args) {
      this->_args.push_back(parseArg(a));
    }
  }
  updateArgs();
  PLOGD << "dcd: " << command_code.toString();
  for (const auto a : _args) {
    PLOGD << " " << a->toString();
  }
  PLOGD << std::endl;
}

bool Command::isLDICmd() {
  return (command_code == CmdCode::LDI) || (command_code == CmdCode::LDIR) || (command_code == CmdCode::LDD) || (command_code == CmdCode::LDDR);
}

void Command::updateArgs() {
  if ((command_code == CmdCode::LD) || (command_code == CmdCode::SBC) || (command_code == CmdCode::ADD) || (command_code == CmdCode::ADC)) {
    auto& a1 = _args[0];
    auto& a2 = _args[1];
    switch (a1->arg_type) {
      case ArgType::ARG_REGISTER16:
        a2->setSize(ArgSize::Word);
        break;
      case ArgType::ARG_REGISTER8:
        a2->setSize(ArgSize::Byte);
      case ArgType::ARG_MEMORY_REF:
        a1->setSize(a2->getSize());
        break;
      case ArgType::ARG_REGISTER_REF:
        a2->setSize(ArgSize::Byte);
        break;
      case ArgType::ARG_REGISTER_OFFSET:
        a2->setSize(ArgSize::Byte);
        break;
    }
    switch (a2->arg_type) {
      case ArgType::ARG_REGISTER16:
        a1->setSize(a2->getSize());
        break;
      case ArgType::ARG_REGISTER8:
        a1->setSize(a2->getSize());
        break;
      case ArgType::ARG_MEMORY_REF:
        a2->setSize(a1->getSize());
        break;
      case ArgType::ARG_REGISTER_REF:
        a1->setSize(ArgSize::Byte);
        break;
      case ArgType::ARG_REGISTER_OFFSET:
        a1->setSize(ArgSize::Byte);
        break;
        /*          case ArgType::ARG_DEFAULT: {
                    std::shared_ptr<ArgRegisterReference> a2ref = std::static_pointer_cast<ArgRegisterReference>(a2);
                    a2ref->setSize(ArgSize::Byte);
                    break;
                  }*/
                  /*default:
                    throw std::runtime_error("can't determine register pointer size");*/
    }

    if (a1->arg_type == ArgType::ARG_MEMORY_REF) {
      std::shared_ptr<ArgMemoryReference> a1ref = std::static_pointer_cast<ArgMemoryReference>(a1);
      auto lbl = DisassemblerCore::inst().makeData(addr, a1ref->addr, a1ref->size == ArgSize::Byte ? memory::Reference::Type::WRITE_BYTE : memory::Reference::Type::WRITE_WORD);
      a1ref->setLabel(lbl);
    }
    if (a2->arg_type == ArgType::ARG_MEMORY_REF) {
      std::shared_ptr<ArgMemoryReference> a2ref = std::static_pointer_cast<ArgMemoryReference>(a2);
      auto lbl = DisassemblerCore::inst().makeData(addr, a2ref->addr, a2ref->size == ArgSize::Byte ? memory::Reference::Type::WRITE_BYTE : memory::Reference::Type::WRITE_WORD);
      a2ref->setLabel(lbl);
    }
  }
}

bool Command::isSingleByteArgCmd() {
  return ((command_code == CmdCode::AND) || (command_code == CmdCode::OR) || (command_code == CmdCode::XOR) || (command_code == CmdCode::RST) || (command_code == CmdCode::CP) || command_code == CmdCode::SUB);
}

ArgPtr Command::parseArg(const std::string& arg) {
  //check if it is reference
  if (arg.substr(0, 1) == "(") {
    //it's reference
    auto rend = arg.find(")");
    auto ref_str = arg.substr(1, rend - 1);
    auto r16 = Register16::getRegister(ref_str);
    if (r16 != Register16::None) {
      //16 bit register
      return std::make_shared <ArgRegisterReference>(r16);
    } else if (arg.find("-") != arg.npos) {
      //register with offset -
      auto a = utils::split(arg, '-');
      auto r = Register16::getRegister(a[0].substr(1));
      return std::make_shared<ArgRegisterOffset>(r, a[1].substr(0, a[1].length() - 1), false);
    } else if (arg.find("+") != arg.npos) {
      //register with offset -
      auto a = utils::split(arg, '+');
      auto r = Register16::getRegister(a[0].substr(1));
      return std::make_shared<ArgRegisterOffset>(r, a[1].substr(0, a[1].length() - 1), true);
    } else {
      //memory
      uint16_t addr = utils::fromHex(ref_str);
      if ((command_code == CmdCode::OUT) || (command_code == CmdCode::IN)) {
        return std::make_shared <ArgPort>(addr);
      } else {
        return std::make_shared <ArgMemoryReference>(addr, true);
      }
    }
  }
  // check if it's a 8 bit register
  auto r8 = Register8::getRegister(arg);
  if (r8 != Register8::None) {
    return std::make_shared<ArgRegister8>(r8);
  }
  auto r16 = Register16::getRegister(arg);
  if (r16 != Register16::None) {
    return std::make_shared<ArgRegister16>(r16);
  }
  auto f = Flag::getFlag(arg);
  if (f != Flag::None) {
    return std::make_shared<ArgFlag>(f);
  }
  uint16_t v = utils::fromHex(arg);
  if (isSingleByteArgCmd()) {
    return std::make_shared<ArgDefault>(v, ArgSize::Byte, true);
  } else if ((command_code == CmdCode::SET) || (command_code == CmdCode::RES) || (command_code == CmdCode::BIT) || (command_code == CmdCode::IM)) {
    return std::make_shared<ArgDefault>(v, ArgSize::Byte, false);
  } else {
    return std::make_shared<ArgDefault>(v, ArgSize::Word, true);
  }
}

JumpType Command::jumpType(uint16_t& jump_addr) {
  if ((command_code == CmdCode::CALL) || (command_code == CmdCode::RST)) {
    jump_addr = getJmpAddrFromString();
    return JumpType::JT_CALL;
  }
  if (((command_code == CmdCode::JR) || (command_code == CmdCode::JP)) && (getArgsCount() == 1)) {
    const auto& arg1 = getArg(0);
    //FIXME: tmp check, must be removed after debug
    const std::string& arg1str = arg1->toString();
    if (arg1str[0] == '"') {
      throw std::runtime_error("unknown argument");
    }
    if (arg1->arg_type == ArgType::ARG_REGISTER_REF) {
      // posybly need to check register: IX, IY, HL
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return JumpType::JT_RET;
    }
    jump_addr = getJmpAddrFromString();
    return JumpType::JT_JUMP;
  }
  if (((command_code == CmdCode::JR) || (command_code == CmdCode::JP)) && (getArgsCount() == 2)) {
    jump_addr = getJmpAddrFromString();
    return JumpType::JT_COND_JUMP;
  }
  if (command_code == CmdCode::DJNZ) {
    jump_addr = getJmpAddrFromString();
    return JumpType::JT_COND_JUMP;
  }
  if ((command_code == CmdCode::RET) && (getArgsCount() == 1)) {
    return JumpType::JT_COND_RET;
  }
  if ((command_code == CmdCode::RET) && (getArgsCount() == 0)) {
    return JumpType::JT_RET;
  }
  if (command_code == CmdCode::RETI) {
    return JumpType::JT_RET;
  }
  if (command_code == CmdCode::RETN) {
    return JumpType::JT_RET;
  }
  return JumpType::JT_NONE;
}

}
}
