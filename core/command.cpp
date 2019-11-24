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
#include "flag.h"

namespace dasm {
namespace core {

memory::Addr Command::getJmpAddrFromString() const {
  if (args.size() == 1) {
    //get from arg1
    return std::stoi(args[0]->toString(), nullptr, 16);
  } else {
    //get from arg2
    return std::stoi(args[1]->toString(), nullptr, 16);
  }
}

memory::Addr Command::getJmpAddr() const {
  if (args.size() == 1) {
    //get from arg1
    return dynamic_cast<ArgLabel*>(args[0].get())->label->addr;
  } else {
    //get from arg2
    return dynamic_cast<ArgLabel*>(args[1].get())->label->addr;
  }
}

void Command::setJmpAddr(const std::shared_ptr<Label> label) {
  // if label is nullptr, we can't change default type arg to the label (we don't know about it)
  if (label != nullptr) {
    if (args.size() == 1) {
      args[0] = std::make_shared<ArgLabel>(label);
    } else {
      args[1] = std::make_shared<ArgLabel>(label);
    }
  }
}

std::string Command::getArgsString() const {
  if (args.size() == 0) {
    return std::string();
  }
  if (args.size() == 1) {
    return args[0]->toString(); //std::toupper(arg1);
  }
  //  return arg1.toUpper()+", "+arg2.toUpper();
  return args[0]->toString() + ", " + args[1]->toString();
}

std::string Command::getOpcodesString() const {
  std::string tmp;
  auto l1 = len;
  memory::Addr a = addr;
  for (; l1; --l1, ++a) {
    tmp += " " + DisassemblerCore::inst().memory().getByte(a).toString();
  }
  return tmp;
}

Byte Command::opcodes(unsigned long long offs) const {
  return DisassemblerCore::inst().memory().getByte(addr + offs);
}

void Command::parse(std::string& src) {
  std::cout << addr.toString();
  std::cout << "src: " << src << std::endl;
  std::vector<std::string> strlist = utils::split(src, ' ');
  command = strlist[0];
  command_code = CmdCode{ command };
  if (strlist.size() > 1) {
    //has args
    std::vector<std::string> args = utils::split(strlist[1], ',');
    for (const auto& a : args) {
      this->args.push_back(parseArg(a));
    }
  }
  std::cout << "dcd: " << command_code.toString();
  for (const auto a : args) {
    std::cout << " " << a->toString();
  }
  std::cout << std::endl;
}

bool Command::isLDICmd() {
  return (command_code == CmdCode::LDI) || (command_code == CmdCode::LDIR) || (command_code == CmdCode::LDD) || (command_code == CmdCode::LDDR);
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
      return std::make_shared<ArgRegisterOffset>(r, a[1].substr(0, a[1].length()-1), false);
    } else if (arg.find("+") != arg.npos) {
      //register with offset -
      auto a = utils::split(arg, '+');
      auto r = Register16::getRegister(a[0].substr(1));
      return std::make_shared<ArgRegisterOffset>(r, a[1].substr(0, a[1].length() - 1), true);
    } else {
      //memory
      auto addr = utils::hex2int(ref_str);
      return std::make_shared <ArgMemoryReference>(addr);
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
  auto v = utils::hex2int(arg);
  return std::make_shared<ArgDefault>(v, 2);
}
}
}
