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
  explicit Byte(uint8_t b = 0) : _byte(b) {}
  inline std::string toString() const { return utils::byte2hex(_byte); }
  explicit operator uint8_t() const { return _byte; }

  bool operator==(const Byte& b) const { return _byte == b._byte; }
private:
  uint8_t _byte;
};

}
}

#endif
