#pragma once

namespace dasm {
namespace core {

class Flag {
public:
  enum Value : uint8_t {
    None,
    C,
    NC,
    Z,
    NZ,
    P,
    M,
    PO,
    PE,
    S,
    N,
    H
  };

  constexpr Flag(Value reg) : value(reg) {}
  operator Value() const { return value; }

  static Flag getFlag(const std::string& arg) {
    if (arg == "C") {
      return Flag::C;
    } else if (arg == "NC") {
      return Flag::NC;
    } else if (arg == "Z") {
      return Flag::Z;
    } else if (arg == "NZ") {
      return Flag::NZ;
    } else if (arg == "P") {
      return Flag::P;
    } else if (arg == "M") {
      return Flag::M;
    } else if (arg == "PO") {
      return Flag::PO;
    } else if (arg == "PE") {
      return Flag::PE;
    } else if (arg == "S") {
      return Flag::S;
    } else if (arg == "N") {
      return Flag::N;
    } else if (arg == "H") {
      return Flag::H;
    } else {
      return Flag::None;
    }
  }


  std::string toString() const {
    switch (value) {
      case None:
        return "";
      case C:
        return "C";
      case NC:
        return "NC";
      case Z:
        return "Z";
      case NZ:
        return "NZ";
      case P:
        return "P";
      case M:
        return "M";
      case PO:
        return "PO";
      case PE:
        return "PE";
      case S:
        return "S";
      case N:
        return "N";
      case H:
        return "H";
      default:
        return "";
    }
  }
private:
  Value value;
};

}
}
