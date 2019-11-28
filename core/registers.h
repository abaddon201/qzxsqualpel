#pragma once

namespace dasm {
namespace core {

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
    if (arg == "A") {
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


class Register16 {
public:
  enum Value : uint8_t {
    None,
    AF,
    AF2,
    BC,
    DE,
    HL,
    SP,
    IX,
    IY
  };

  constexpr Register16(Value reg) : value(reg) {}
  operator Value() const { return value; }

  Register8 getHight() {
    switch (value) {
      case HL:
        return Register8::H;
      case AF:
        return Register8::A;
      case BC:
        return Register8::B;
      case DE:
        return Register8::D;
      case IX:
        return Register8::IXh;
      case IY:
        return Register8::IYh;
    }
    return Register8::None;
  }

  Register8 getLow() {
    switch (value) {
      case HL:
        return Register8::L;
      case AF:
        return Register8::F;
      case BC:
        return Register8::C;
      case DE:
        return Register8::E;
      case IX:
        return Register8::IXl;
      case IY:
        return Register8::IYl;
    }
    return Register8::None;
  }

  bool isPart(Register8 reg) {
    switch (value) {
      case HL:
        return (reg == Register8::L) || (reg == Register8::H);
      case AF:
        return (reg == Register8::F) || (reg == Register8::A);
      case BC:
        return (reg == Register8::C) || (reg == Register8::B);
      case DE:
        return (reg == Register8::E) || (reg == Register8::D);
      case IX:
        return (reg == Register8::IXl) || (reg == Register8::IXh);
      case IY:
        return (reg == Register8::IYl) || (reg == Register8::IYh);
    }
    return false;
  }

  static Register16 getRegister(const std::string& arg) {
    if (arg=="AF") {
      return Register16::AF;
    } else if (arg == "AF'") {
      return Register16::AF2;
    } else if (arg == "BC") {
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
      case AF2:
        return "AF'";
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
