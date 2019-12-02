#pragma once

#include "core/disassembler_core.h"

namespace dasm {
namespace files {
namespace project {

class Serializer {
public:
  static std::string serialize(const core::DisassemblerCore& core);

  static void deserialize_file(const std::string& file_name, core::DisassemblerCore& core);
};

}
}
}
