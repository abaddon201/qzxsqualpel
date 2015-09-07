//
// C++ Implementation: CCommand
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "CCommand.h"
#include "IDisassemblerCore.h"
#include "arg_label.h"

CAddr CCommand::getJmpAddrFromString() const {
  if (arg2==nullptr) {
    //get from arg1
    return std::stoi(arg1->toString(), nullptr, 16);
  } else {
    //get from arg2
    return std::stoi(arg2->toString(), nullptr, 16);
  }
}

CAddr CCommand::getJmpAddr() const {
  if (arg2==nullptr) {
    //get from arg1
    return dynamic_cast<ArgLabel*>(arg1.get())->label->addr;
  } else {
    //get from arg2
    return dynamic_cast<ArgLabel*>(arg2.get())->label->addr;
  }
}

void CCommand::setJmpAddr(const std::shared_ptr<CLabel> label) {
  // if label is nullptr, we can't change default type arg to the label (we don't know about it)
  if (label != nullptr) {
    if (arg2==nullptr) {
      arg1 = std::make_shared<ArgLabel>(label);
    } else {
      arg2 = std::make_shared<ArgLabel>(label);
    }
  }
}

std::string CCommand::getArgsString() const {
  if (arg1==nullptr) {
    return std::string();
  }
  if (arg2==nullptr) {
    return arg1->toString(); //std::toupper(arg1);
  }
//  return arg1.toUpper()+", "+arg2.toUpper();
  return arg1->toString()+", "+arg2->toString();
}

std::string CCommand::getOpcodesString() const {
  std::string tmp;
  int l1=len;
  CAddr a = addr;
  for (; l1; --l1, ++a) {
    tmp += " " + IDisassemblerCore::inst()->getMemoryByte(a).toString();
  }
  return tmp;
}

CByte CCommand::opcodes(unsigned long long offs) const {
  return IDisassemblerCore::inst()->getMemoryByte(addr + offs);
}
