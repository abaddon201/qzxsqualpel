//
// C++ Implementation: CAddr
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "addr.h"
#include "utils/utils.h"

namespace dasm {
namespace memory {

const std::string& Addr::toString() const {
  if (_dirty) {
    _hex_cache = utils::hexify(_segment, 4) + ":" + utils::hexify(_offset, 4);
    _dirty = false;
  }
  return _hex_cache;
}

std::string Addr::offsetString() const {
  return utils::hexify(_offset, 4);
}

}
}
