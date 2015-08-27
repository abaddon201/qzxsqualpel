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

CReference::CReference(CAddr addr, ReferenceType type)
  : m_From(addr), m_Type(type) {
}

QString CReference::toString() {
  QString res="; XRef: ";
  res+=CDisassembler::inst()->labels().offsetInLabel(m_From);
  switch (m_Type) {
  case JUMP:
    res+="/j";
    break;
  case CALL:
    res+="/c";
    break;
  case READ_BYTE:
    res+="/rb";
    break;
  case WRITE_BYTE:
    res+="/wb";
    break;
  case READ_WORD:
    res+="/rw";
    break;
  case WRITE_WORD:
    res+="/ww";
    break;
  };
  return res;
}
