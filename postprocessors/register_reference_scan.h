#pragma once

#include "ipostprocessor.h"

namespace dasm {
namespace postprocessors {

class RegisterReferenceScan : public IPostProcessor {
public:
  bool checkPrecondition(core::CommandPtr chunk) override { return true; }

  size_t process(core::CommandPtr chunk, size_t len) override;

private:
  void updateRegisterSource(uint16_t addr, core::ArgPtr arg, core::ArgSize size);
  void updateRegisterSource(uint16_t addr, core::Register16 reg, core::ArgSize size);
  bool isNotBreakCommand(core::CommandPtr cmd);
};

}
}
