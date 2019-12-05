#pragma once

#include "core/command.h"

namespace dasm {
namespace postprocessors {

class IPostProcessor {

public:
  virtual bool checkPrecondition(core::CommandPtr chunk) = 0;

  virtual size_t process(core::CommandPtr chunk, size_t len) = 0;
};
}
}