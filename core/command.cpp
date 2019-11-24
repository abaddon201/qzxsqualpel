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
#include "i_disassembler_core.h"

namespace dasm {
namespace core {

memory::Addr Command::getJmpAddrFromString() const {
  if (arg2 == nullptr) {
    //get from arg1
    return std::stoi(arg1->toString(), nullptr, 16);
  } else {
    //get from arg2
    return std::stoi(arg2->toString(), nullptr, 16);
  }
}

memory::Addr Command::getJmpAddr() const {
  if (arg2 == nullptr) {
    //get from arg1
    return dynamic_cast<ArgLabel*>(arg1.get())->label->addr;
  } else {
    //get from arg2
    return dynamic_cast<ArgLabel*>(arg2.get())->label->addr;
  }
}

void Command::setJmpAddr(const std::shared_ptr<Label> label) {
  // if label is nullptr, we can't change default type arg to the label (we don't know about it)
  if (label != nullptr) {
    if (arg2 == nullptr) {
      arg1 = std::make_shared<ArgLabel>(label);
    } else {
      arg2 = std::make_shared<ArgLabel>(label);
    }
  }
}

std::string Command::getArgsString() const {
  if (arg1 == nullptr) {
    return std::string();
  }
  if (arg2 == nullptr) {
    return arg1->toString(); //std::toupper(arg1);
  }
  //  return arg1.toUpper()+", "+arg2.toUpper();
  return arg1->toString() + ", " + arg2->toString();
}

std::string Command::getOpcodesString() const {
  std::string tmp;
  auto l1 = len;
  memory::Addr a = addr;
  for (; l1; --l1, ++a) {
    tmp += " " + DisassemblerCore::inst()->getMemoryByte(a).toString();
  }
  return tmp;
}

Byte Command::opcodes(unsigned long long offs) const {
  return DisassemblerCore::inst()->getMemoryByte(addr + offs);
}

bool Command::isLDICmd() {
  return (command == "LDI") || (command == "LDIR") || (command == "LDD") || (command == "LDDR");
}


}
}
