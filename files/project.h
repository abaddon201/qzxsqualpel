#pragma once

#include "core/disassembler_core.h"

namespace dasm {
namespace files {
namespace project {

class Serializer {
public:
  static std::string serialize(const core::DisassemblerCore& core);

  static void deserialize(const std::string& src, core::DisassemblerCore& core);
};

}
}
}
