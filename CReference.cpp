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
#include "CDisassembler.h"

CReference::CReference(CAddr addr, Type type)
  : m_From(addr), m_Type(type) {
}

QString CReference::toString() {
  QString res="; XRef: ";
  res+=CDisassembler::core_inst()->labels().offsetInLabel(m_From);
  switch (m_Type) {
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
