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

#include "CReference.h"
#include "CLabels.h"
#include "IDisassemblerCore.h"

CReference::CReference(CAddr addr_, Type type_)
  : addr(addr_), type(type_) {
}

QString CReference::toString() {
  QString res="; XRef: ";
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
