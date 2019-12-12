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
#include <vector>

#include "memory/reference.h"

namespace dasm {
namespace core {

struct Label {
  Label() = default;

  Label(uint16_t addr, const std::string& name) : name(std::move(name)), addr(addr) {}

  Label(const Label& s) {
    addr = s.addr;
    name = s.name;
    _references = s._references;
  }

  Label& operator=(const Label& s) {
    addr = s.addr;
    name = s.name;
    _references = s._references;
    return *this;
  }

  ~Label() = default;

  using ReferencesList = std::vector<memory::ReferencePtr>;

  void addCrossRef(uint16_t addr, memory::Reference::Type type);
  ReferencesList& references() { return _references; }

  std::string name;
  uint16_t addr;
private:
  ReferencesList _references;
};

using LabelPtr = std::shared_ptr<Label>;
}
}

#endif
