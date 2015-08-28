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

QString CByte::toString() const {
  QString tmp2=QString::number(m_Byte, 16);
  int dop=2-tmp2.length();
  while (dop) {
    tmp2.insert(0, "0");
    dop--;
  }
  return tmp2.toUpper();
}
