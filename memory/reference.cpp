//
// C++ Implementation: CRefernce
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "reference.h"
#include "core/labels.h"
#include "core/i_disassembler_core.h"

namespace dasm {
namespace memory {

Reference::Reference(const Addr& addr_, Type type_)
  : addr(addr_), type(type_) {}

std::string Reference::toString() {
  std::string res = "; XRef: ";
  res += core::DisassemblerCore::inst()->labels().offsetInLabel(addr);
  switch (type) {
    case Type::JUMP:
      res += "/j";
      break;
    case Type::CALL:
      res += "/c";
      break;
    case Type::READ_BYTE:
      res += "/rb";
      break;
    case Type::WRITE_BYTE:
      res += "/wb";
      break;
    case Type::READ_WORD:
      res += "/rw";
      break;
    case Type::WRITE_WORD:
      res += "/ww";
      break;
  };
  return res;
}

}
}
