//
// C++ Interface: CRefernce
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CREFERENCE_H
#define CREFERENCE_H

#include "CAddr.h"

class CReference {
public:
  enum class Type {
    JUMP=1,
    CALL,
    READ_BYTE,
    WRITE_BYTE,
    READ_WORD,
    WRITE_WORD
  };
public:
  CReference(CAddr addr, Type type);
  QString toString();
private:
  CAddr m_From;
  Type m_Type;
};

#endif
