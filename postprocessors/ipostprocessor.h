#pragma once

#include "core/chunk.h"

namespace dasm {
namespace postprocessors {

class IPostProcessor {

public:
  virtual bool checkPrecondition(std::shared_ptr<core::Chunk> chunk) = 0;

  virtual size_t process(std::shared_ptr<core::Chunk> chunk, size_t len) = 0;
};
}
}