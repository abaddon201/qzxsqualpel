//
// C++ Interface: CLabel
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABEL_H
#define CLABEL_H

#include <utility>

#include "addr.h"

namespace dasm {
namespace core {

struct Label {
  Label() = default;

  Label(const Addr& addr, std::string name) : name(std::move(name)), addr(addr) {}

  Label(const Label& s) {
    addr = s.addr;
    name = s.name;
  }

  Label& operator=(const Label& s) {
    addr = s.addr;
    name = s.name;
    return *this;
  }

  ~Label() = default;

  std::string name;
  Addr addr;
};

}
}

#endif
