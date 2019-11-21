#pragma once 

enum class ZXJumpCmd {
  CMD_NONE = 0,
  CMD_CALL,
  CMD_RST,
  CMD_RET,
  CMD_RETI,
  CMD_RETN,
  CMD_JP,
  CMD_JR
};
