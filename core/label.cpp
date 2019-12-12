//
// C++ Implementation: CLabel
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "label.h"

namespace dasm {
namespace core {

void Label::addCrossRef(uint16_t addr, memory::Reference::Type type) {
  memory::ReferencePtr ref = std::make_shared<memory::Reference>(addr, type);
  _references.push_back(ref);
}

}
}