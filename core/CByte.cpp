//
// C++ Implementation: CByte
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "CByte.h"
#include "utils.h"

std::string CByte::toString() const {
  std::string tmp2=hexify<unsigned char>(m_Byte, 2);
  return tmp2/*.toUpper()*/;
}
