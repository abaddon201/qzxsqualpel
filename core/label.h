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
#include <memory>
#include <string>

namespace dasm {
namespace core {

struct Label {
  Label() = default;

  Label(uint16_t addr, const std::string& name) : name(std::move(name)), addr(addr) {}

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
  uint16_t addr;
};

using LabelPtr = std::shared_ptr<Label>;
}
}

#endif
