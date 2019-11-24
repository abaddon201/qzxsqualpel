#pragma once

namespace dasm {
namespace core {

class Register16 {
public:
  enum Value : uint8_t {
    None,
    AF,
    BC,
    DE,
    HL,
    SP,
    IX,
    IY
  };

  constexpr Register16(Value reg) : value(reg) {}
  operator Value() const { return value; }

  static Register16 getRegister(const std::string& arg) {
    if (arg.find("AF") != arg.npos) {
      return Register16::AF;
    } else if (arg.find("BC") != arg.npos) {
      return Register16::BC;
    } else if (arg.find("DE") != arg.npos) {
      return Register16::DE;
    } else if (arg.find("HL") != arg.npos) {
      return Register16::HL;
    } else if (arg.find("SP") != arg.npos) {
      return Register16::SP;
    } else if (arg.find("IX") != arg.npos) {
      return Register16::IX;
    } else if (arg.find("IY") != arg.npos) {
      return Register16::IY;
    } else {
      return Register16::None;
    }
  }


  std::string toString() const {
    switch (value) {
      case None:
        return "";
      case AF:
        return "AF";
      case BC:
        return "BC";
      case DE:
        return "DE";
      case HL:
        return "HL";
      case SP:
        return "SP";
      case IX:
        return "IX";
      case IY:
        return "IY";
      default:
        return "";
    }
  }
private:
  Value value;
};


}
}
