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
    NONE = 0,
    CALL,
    RST,
    RET,
    RETI,
    RETN,
    JP,
    JR,
    DJNZ,
    LD,
    LDI,
    LDD,
    LDIR,
    LDDR,
    IN,
    OUT,
    SET,
    RES,
    BIT,
    CP,
    AND,
    OR,
    XOR
  };

  CmdCode() : value{ NONE } {}

  CmdCode(const std::string& cmd) {
    if (cmd == "CALL") {
      value = CALL;
    } else if (cmd == "RST") {
      value = RST;
    } else if (cmd == "RET") {
      value = RET;
    } else if (cmd == "RETI") {
      value = RETI;
    } else if (cmd == "RETN") {
      value = RETN;
    } else if (cmd == "JP") {
      value = JP;
    } else if (cmd == "JR") {
      value = JR;
    } else if (cmd == "DJNZ") {
      value = DJNZ;
    } else if (cmd == "LD") {
      value = LD;
    } else if (cmd == "LDI") {
      value = LDI;
    } else if (cmd == "LDD") {
      value = LDD;
    } else if (cmd == "LDIR") {
      value = LDIR;
    } else if (cmd == "LDDR") {
      value = LDDR;
    } else if (cmd == "IN") {
      value = IN;
    } else if (cmd == "OUT") {
      value = OUT;
    } else if (cmd == "SET") {
      value = SET;
    } else if (cmd == "RES") {
      value = RES;
    } else if (cmd == "BIT") {
      value = BIT;
    } else if (cmd == "CP") {
      value = CP;
    } else if (cmd == "AND") {
      value = AND;
    } else if (cmd == "OR") {
      value = OR;
    } else if (cmd == "XOR") {
      value = XOR;
    } else {
      value = NONE;
    }
  }

  std::string toString() {
    switch (value) {
      case CALL:
        return "CALL";
      case RST:
        return "RST";
      case RET:
        return "RET";
      case RETI:
        return "RETI";
      case RETN:
        return "RETN";
      case JP:
        return "JP";
      case JR:
        return "JR";
      case DJNZ:
        return "DJNZ";
      case LD:
        return "LD";
      case LDI:
        return "LDI";
      case LDD:
        return "LDD";
      case LDIR:
        return "LDIR";
      case LDDR:
        return "LDDR";
      case IN:
        return "IN";
      case OUT:
        return "OUT";
      case SET:
        return "SET";
      case RES:
        return "RES";
      case BIT:
        return "BIT";
      case CP:
        return "CP";
      case AND:
        return "AND";
      case OR:
        return "OR";
      case XOR:
        return "XOR";
    }
    return "";
  }

  constexpr CmdCode(Value cmd) : value(cmd) {}
  operator Value() const { return value; }

private:
  Value value;
};

}
}
