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

CAddr CCommand::getJmpAddr() const {
  if (arg2.empty()) {
    //get from arg1
    return std::stoi(arg1, nullptr, 16);
  } else {
    //get from arg2
    return std::stoi(arg2, nullptr, 16);
  }
}

void CCommand::setJmpAddr(const std::string label) {
  if (arg2.empty()) {
    arg1 = label;
  } else {
    arg2 = label;
  }
}

std::string CCommand::getArgsString() const {
  if (arg1.empty()) {
    return std::string();
  }
  if (arg2.empty()) {
    return arg1; //std::toupper(arg1);
  }
//  return arg1.toUpper()+", "+arg2.toUpper();
  return arg1+", "+arg2;
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
