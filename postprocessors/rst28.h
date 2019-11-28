#pragma once

#include "ipostprocessor.h"

#include "core/chunk.h"

namespace dasm {
namespace postprocessors {

class Rst28 : public IPostProcessor {
public:
  bool checkPrecondition(core::ChunkPtr chunk) override;

  size_t process(core::ChunkPtr chunk, size_t len) override;

private:
  std::string getRST28AutoComment(unsigned char b, int& args_count);
};

}
}
