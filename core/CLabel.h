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

#include "addr.h"

struct Label {
  Label() {}
  Label(Addr addr, std::string name) : name(name), addr(addr) {}

  Label(const Label &s) {
    addr=s.addr;
    name=s.name;
  }
  Label &operator = (const Label &s) {
    addr=s.addr;
    name=s.name;
    return *this;
  }
  ~Label() {}

  std::string name;
  Addr addr;
};

#endif
