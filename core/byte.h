//
// C++ Interface: CByte
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2019
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CBYTE_H
#define CBYTE_H

#include <string>
#include "utils/utils.h"

namespace dasm {
namespace core {

class Byte {
public:
  explicit Byte(unsigned char b = 0) : _byte(b) {}
  inline std::string toString() const { return byte2hex(_byte); }
  explicit operator unsigned char() const { return _byte; }

private:
  unsigned char _byte;
};

}
}

#endif
