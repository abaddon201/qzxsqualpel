#pragma once

namespace dasm {
namespace core {

enum class JumpType {
  JT_NONE = 0,
  JT_CALL,
  JT_COND_JUMP,
  JT_JUMP,
  JT_COND_RET,
  JT_RET,
};

enum class JumpCmd {
  CMD_NONE = 0,
  CMD_CALL,
  CMD_RST,
  CMD_RET,
  CMD_RETI,
  CMD_RETN,
  CMD_JP,
  CMD_JR
};

}
}