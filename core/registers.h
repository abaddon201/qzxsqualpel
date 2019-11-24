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
    if (arg=="AF") {
      return Register16::AF;
    } else if (arg=="BC") {
      return Register16::BC;
    } else if (arg=="DE") {
      return Register16::DE;
    } else if (arg=="HL") {
      return Register16::HL;
    } else if (arg=="SP") {
      return Register16::SP;
    } else if (arg=="IX") {
      return Register16::IX;
    } else if (arg=="IY") {
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



class Register8 {
public:
  enum Value : uint8_t {
    None,
    A,
    F,
    B,
    C,
    D,
    E,
    H,
    L,
    IXh,
    IXl,
    IYh,
    IYl,
    I,
    R
  };

  constexpr Register8(Value reg) : value(reg) {}
  operator Value() const { return value; }

  static Register8 getRegister(const std::string& arg) {
    if (arg=="A") {
      return Register8::A;
    } else if (arg == "F") {
      return Register8::F;
    } else if (arg == "B") {
      return Register8::B;
    } else if (arg == "C") {
      return Register8::C;
    } else if (arg == "D") {
      return Register8::D;
    } else if (arg == "E") {
      return Register8::E;
    } else if (arg == "H") {
      return Register8::H;
    } else if (arg == "L") {
      return Register8::L;
    } else if (arg == "IXh") {
      return Register8::IXh;
    } else if (arg == "IXl") {
      return Register8::IXl;
    } else if (arg == "IYh") {
      return Register8::IYh;
    } else if (arg == "IYl") {
      return Register8::IYl;
    } else if (arg == "I") {
      return Register8::I;
    } else if (arg == "R") {
      return Register8::R;
    } else {
      return Register8::None;
    }
  }


  std::string toString() const {
    switch (value) {
      case None:
        return "";
      case A:
        return "A";
      case F:
        return "F";
      case B:
        return "B";
      case C:
        return "C";
      case D:
        return "D";
      case E:
        return "E";
      case H:
        return "H";
      case L:
        return "L";
      case IXh:
        return "IXh";
      case IXl:
        return "IXl";
      case IYh:
        return "IYh";
      case IYl:
        return "IYl";
      case I:
        return "I";
      case R:
        return "R";
      default:
        return "";
    }
  }
private:
  Value value;
};


}
}
