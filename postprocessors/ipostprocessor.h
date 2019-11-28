#pragma once

#include "core/chunk.h"

namespace dasm {
namespace postprocessors {

class IPostProcessor {

public:
  virtual bool checkPrecondition(core::ChunkPtr chunk) = 0;

  virtual size_t process(core::ChunkPtr chunk, size_t len) = 0;
};
}
}