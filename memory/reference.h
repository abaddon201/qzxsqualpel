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

#include <memory>
#include <string>

namespace dasm {
namespace memory {

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

  Reference() : type{ Type::UNKNOWN } {}
  Reference(const uint16_t addr_, Type type_);

  std::string toString();

  uint16_t addr;
  Type type;
};

using ReferencePtr = std::shared_ptr<Reference>;
}
}

#endif
