//
// C++ Implementation: CRefernce
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "reference.h"
#include "labels.h"
#include "i_disassembler_core.h"

Reference::Reference(Addr addr_, Type type_)
  : addr(addr_), type(type_) {
}

std::string Reference::toString() {
  std::string res="; XRef: ";
  res+=IDisassemblerCore::inst()->labels().offsetInLabel(addr);
  switch (type) {
  case Type::JUMP:
    res+="/j";
    break;
  case Type::CALL:
    res+="/c";
    break;
  case Type::READ_BYTE:
    res+="/rb";
    break;
  case Type::WRITE_BYTE:
    res+="/wb";
    break;
  case Type::READ_WORD:
    res+="/rw";
    break;
  case Type::WRITE_WORD:
    res+="/ww";
    break;
  };
  return res;
}
