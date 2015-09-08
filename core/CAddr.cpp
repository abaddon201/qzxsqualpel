//
// C++ Implementation: CAddr
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "CAddr.h"
#include "utils.h"

const std::string& Addr::toString() const {
  if (_dirty) {
    _hex_cache = hexify(_segment, 4) + ":" + hexify(_offset, 4);
    _dirty = false;
  }
  return _hex_cache;
}

std::string Addr::offsetString() const {
  return hexify(_offset, 4);
}
