//
// C++ Interface: CRefernce
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CREFERENCE_H
#define CREFERENCE_H

#include "addr.h"

namespace dasm {
namespace core {

struct Reference {
  enum class Type {
    UNKNOWN = 0,
    JUMP = 1,
    CALL,
    READ_BYTE,
    WRITE_BYTE,
    READ_WORD,
    WRITE_WORD
  };

  Reference(const Addr& addr_, Type type_);

  std::string toString();

  Addr addr;
  Type type;
};

}
}

#endif
