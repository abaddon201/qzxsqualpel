#pragma once

#include "ipostprocessor.h"

#include "core/chunk.h"

namespace dasm {
namespace postprocessors {

class RegisterReferenceScan : public IPostProcessor {
public:
  bool checkPrecondition(core::ChunkPtr chunk) override { return true; }

  size_t process(core::ChunkPtr chunk, size_t len) override;

private:
  void updateRegisterSource(core::ChunkPtr chunk, int idx, core::ArgPtr arg, core::ArgSize size);
  void updateRegisterSource(core::ChunkPtr chunk, int idx, core::Register16 reg, core::ArgSize size);
};

}
}
