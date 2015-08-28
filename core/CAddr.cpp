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

std::string CAddr::toString() const {
  std::string tmp = hexify(m_Segment, 4) + ":" + hexify(m_Offset, 4);
  return tmp;
}

std::string CAddr::offsetString() const {
  return hexify(m_Offset, 4);
}
