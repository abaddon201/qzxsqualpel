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

const std::string& CAddr::toString() const {
  if (_dirty) {
    _hex_cache = hexify(m_Segment, 4) + ":" + hexify(m_Offset, 4);
    _dirty = false;
  }
  return _hex_cache;
}

std::string CAddr::offsetString() const {
  return hexify(m_Offset, 4);
}
