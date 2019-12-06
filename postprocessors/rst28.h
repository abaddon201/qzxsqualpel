#pragma once

#include "ipostprocessor.h"

namespace dasm {
namespace postprocessors {

class Rst28 : public IPostProcessor {
public:
  bool checkPrecondition(core::CommandPtr cmd) override;

  size_t process(core::CommandPtr cmd, size_t len) override;

private:
  std::string getRST28AutoComment(unsigned char b, int& args_count);
};

}
}
