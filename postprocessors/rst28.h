#pragma once

#include "ipostprocessor.h"

#include "core/chunk.h"

namespace dasm {
namespace postprocessors {

class Rst28 : public IPostProcessor {
public:
  bool checkPrecondition(std::shared_ptr<core::Chunk> chunk) override;

  size_t process(std::shared_ptr<core::Chunk> chunk, size_t len) override;

private:
  std::string getRST28AutoComment(unsigned char b, int& args_count);
};

}
}