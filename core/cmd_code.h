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

class CmdCode {
public:
  enum Value : uint8_t {
    CMD_NONE = 0,
    CMD_CALL,
    CMD_RST,
    CMD_RET,
    CMD_RETI,
    CMD_RETN,
    CMD_JP,
    CMD_JR
  };

  CmdCode() : value{ CMD_NONE } {}

  CmdCode(const std::string& cmd) {
    if (cmd == "CALL") {
      value = CMD_CALL;
    } else if (cmd == "RST") {
      value = CMD_RST;
    } else if (cmd == "RET") {
      value = CMD_RET;
    } else if (cmd == "RETI") {
      value = CMD_RETI;
    } else if (cmd == "RETN") {
      value = CMD_RETN;
    } else if (cmd == "JP") {
      value = CMD_JP;
    } else if (cmd == "JR") {
      value = CMD_JR;
    } else {
      value = CMD_NONE;
    }
  }

  constexpr CmdCode(Value cmd) : value(cmd) {}
  operator Value() const { return value; }

private:
  Value value;
};

}
}
