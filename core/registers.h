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
