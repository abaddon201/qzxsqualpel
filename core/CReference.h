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

struct CReference {
  enum class Type {
    JUMP=1,
    CALL,
    READ_BYTE,
    WRITE_BYTE,
    READ_WORD,
    WRITE_WORD
  };
  CReference(CAddr addr_, Type type_);
  std::string toString();

  CAddr addr;
  Type type;
};

#endif
