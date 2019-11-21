#pragma once

#include "command.h"
#include "chunk.h"

namespace dasm {
namespace core {

class AutoCommenter {
public:
  virtual void commentCommand(Command& out_command) = 0;

  virtual ~AutoCommenter() = default;

};

}
}
