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

QString CAddr::toString() const {
  QString tmp=QString::number(m_Segment, 16)+":";
  QString tmp2=QString::number(m_Offset, 16);
  int dop=4-tmp2.length();
  while (dop) {
    tmp2.insert(0, "0");
    dop--;
  }
  tmp+=tmp2;
  return tmp.toUpper();
}

QString CAddr::offsetString() const {
  QString tmp2=QString::number(m_Offset, 16);
  int dop=4-tmp2.length();
  while (dop) {
    tmp2.insert(0, "0");
    dop--;
  }
  return tmp2.toUpper();
}
