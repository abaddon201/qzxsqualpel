#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

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
    XOR,
    SUB,
    SLA,
    SRA,
    SRL,
    CCF,
    SCF,
    SBC,
    RR,
    RRC,
    RRA,
    RRCA,
    RRD,
    RL,
    RLA,
    RLC,
    RLCA,
    RLD,
    PUSH,
    POP,
    INI,
    IND,
    INIR,
    INDR,
    OUTI,
    OUTD,
    OTIR,
    OTDR,
    NOP,
    NEG,
    INC,
    DEC,
    IM,
    HALT,
    EXX,
    EX,
    EI,
    DI,
    DAA,
    CPL,
    CPI,
    CPD,
    CPIR,
    CPDR,
    ADD,
    ADC,
    DB,
    DW
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
    } else if (cmd == "SUB") {
      value = SUB;
    } else if (cmd == "SLA") {
      value = SLA;
    } else if (cmd == "SRA") {
      value = SRA;
    } else if (cmd == "SRL") {
      value = SRL;
    } else if (cmd == "SCF") {
      value = SCF;
    } else if (cmd == "CCF") {
      value = CCF;
    } else if (cmd == "SBC") {
      value = SBC;
    } else if (cmd == "RL") {
      value = RL;
    } else if (cmd == "RLA") {
      value = RLA;
    } else if (cmd == "RLC") {
      value = RLC;
    } else if (cmd == "RLCA") {
      value = RLCA;
    } else if (cmd == "RLD") {
      value = RLD;
    } else if (cmd == "RR") {
      value = RR;
    } else if (cmd == "RRC") {
      value = RRC;
    } else if (cmd == "RRA") {
      value = RRA;
    } else if (cmd == "RRCA") {
      value = RRCA;
    } else if (cmd == "RRD") {
      value = RRD;
    } else if (cmd == "PUSH") {
      value = PUSH;
    } else if (cmd == "POP") {
      value = POP;
    } else if (cmd == "INI") {
      value = INI;
    } else if (cmd == "IND") {
      value = IND;
    } else if (cmd == "INIR") {
      value = INIR;
    } else if (cmd == "INDR") {
      value = INDR;
    } else if (cmd == "OUTI") {
      value = OUTI;
    } else if (cmd == "OUTD") {
      value = OUTD;
    } else if (cmd == "OTIR") {
      value = OTIR;
    } else if (cmd == "OTDR") {
      value = OTDR;
    } else if (cmd == "NOP") {
      value = NOP;
    } else if (cmd == "NEG") {
      value = NEG;
    } else if (cmd == "INC") {
      value = INC;
    } else if (cmd == "DEC") {
      value = DEC;
    } else if (cmd == "IM") {
      value = IM;
    } else if (cmd == "HALT") {
      value = HALT;
    } else if (cmd == "EXX") {
      value = EXX;
    } else if (cmd == "EX") {
      value = EX;
    } else if (cmd == "EI") {
      value = EI;
    } else if (cmd == "DI") {
      value = DI;
    } else if (cmd == "DAA") {
      value = DAA;
    } else if (cmd == "CPL") {
      value = CPL;
    } else if (cmd == "CPI") {
      value = INI;
    } else if (cmd == "CPD") {
      value = IND;
    } else if (cmd == "CPIR") {
      value = INIR;
    } else if (cmd == "CPDR") {
      value = INDR;
    } else if (cmd == "ADD") {
      value = ADD;
    } else if (cmd == "ADC") {
      value = ADC;
    } else if (cmd == "DB") {
      value = DB;
    } else if (cmd == "DW") {
      value = DW;
    } else if (cmd == "db") {
      value = NONE;
    } else {
      value = NONE;
      throw std::runtime_error("Unknown command: " + cmd);
    }
  }

  std::string toString() const {
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
      case SUB:
        return "SUB";
      case SLA:
        return "SLA";
      case SRA:
        return "SRA";
      case SRL:
        return "SRL";
      case SCF:
        return "SCF";
      case CCF:
        return "CCF";
      case SBC:
        return "SBC";
      case RL:
        return "RL";
      case RLC:
        return "RLC";
      case RLA:
        return "RLA";
      case RLCA:
        return "RLCA";
      case RLD:
        return "RLD";
      case RR:
        return "RR";
      case RRC:
        return "RRC";
      case RRA:
        return "RRA";
      case RRCA:
        return "RRCA";
      case RRD:
        return "RRD";
      case PUSH:
        return "PUSH";
      case POP:
        return "POP";
      case OUTI:
        return "OUTI";
      case OUTD:
        return "OUTD";
      case OTIR:
        return "OTIR";
      case OTDR:
        return "OTDR";
      case INI:
        return "INI";
      case IND:
        return "IND";
      case INIR:
        return "INIR";
      case INDR:
        return "INDR";
      case NOP:
        return "NOP";
      case NEG:
        return "NEG";
      case INC:
        return "INC";
      case DEC:
        return "DEC";
      case IM:
        return "IM";
      case HALT:
        return "HALT";
      case EXX:
        return "EXX";
      case EX:
        return "EX";
      case EI:
        return "EI";
      case DI:
        return "DI";
      case DAA:
        return "DAA";
      case CPL:
        return "CPL";
      case CPI:
        return "CPI";
      case CPD:
        return "CPD";
      case CPIR:
        return "CPIR";
      case CPDR:
        return "CPDR";
      case ADD:
        return "ADD";
      case ADC:
        return "ADC";
      case DB:
        return "DB";
      case DW:
        return "DW";
    }
    return "db"; //unknown/unparsed
  }

  constexpr CmdCode(Value cmd) : value(cmd) {}
  operator Value() const { return value; }

private:
  Value value;
};

}
}
