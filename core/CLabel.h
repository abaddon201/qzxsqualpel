//
// C++ Interface: CLabel
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABEL_H
#define CLABEL_H

#include "CAddr.h"

struct CLabel {
  CLabel() {}
  CLabel(CAddr addr, QString name) : name(name), addr(addr) {}

  CLabel(const CLabel &s) {
    addr=s.addr;
    name=s.name;
  }
  CLabel &operator = (const CLabel &s) {
    addr=s.addr;
    name=s.name;
    return *this;
  }
  ~CLabel() {}

  QString name;
  CAddr addr;
};

#include <QDebug>
inline void operator<<(QDebug out, CLabel lab) {
  out<<"Label: Name='"<<lab.name<<"', addr="<<lab.addr.toString();
}

#endif
