#include "CDisassemblerCoreZX.h"

void CDisassemblerCoreZX::autoCommentCommand(CCommand &out_command) {
  if (out_command.command=="RST") {
    //known RST's
    if (out_command.arg1=="18") {
      out_command.auto_comment="NEXT_CHAR";
    } else if (out_command.arg1=="20") {
      out_command.auto_comment="GET_CHAR";
    } else if (out_command.arg1=="28") {
      out_command.auto_comment="FP_CALC";
    }
  } else if (out_command.command=="CALL") {
    //known CALL's
  }
}

std::string CDisassemblerCoreZX::getRST28AutoComment(unsigned char b) {
  switch (b) {
  case 0x00:
    return "jump-true";
  case 0x01:
    return "exchange";
  case 0x02:
    return "delete";
  case 0x03:
    return "substract";
  case 0x04:
    return "multiply"; //1 byte arg
  case 0x05:
    return "division";
  case 0x06:
    return "to-power";
  case 0x07:
    return "or";
  case 0x08:
    return "no-&-no";
  case 0x09:
    return "no-l-eql";
  case 0x0A:
    return "no-gr-eq";
  case 0x0B:
    return "nos-neql";
  case 0x0C:
    return "no-grtr";
  case 0x0D:
    return "no-less";
  case 0x0E:
    return "nos-eql";
  case 0x0F:
    return "addition";
  case 0x10:
    return "str-&-no";
  case 0x11:
    return "str-l-eql";
  case 0x12:
    return "str-gr-eq";
  case 0x13:
    return "strs-neql";
  case 0x14:
    return "str-grtr";
  case 0x15:
    return "str-less";
  case 0x16:
    return "strs-eql";
  case 0x17:
    return "strs-add";
  case 0x18:
    return "val$";
  case 0x19:
    return "usr-$";
  case 0x1A:
    return "read-in";
  case 0x1B:
    return "negate";
  case 0x1C:
    return "code";
  case 0x1D:
    return "val";
  case 0x1E:
    return "len";
  case 0x1F:
    return "sin";
  case 0x20:
    return "cos";
  case 0x21:
    return "tan";
  case 0x22:
    return "asn";
  case 0x23:
    return "acs";
  case 0x24:
    return "atn";
  case 0x25:
    return "ln";
  case 0x26:
    return "exp";
  case 0x27:
    return "int";
  case 0x28:
    return "sqr";
  case 0x29:
    return "sgn";
  case 0x2A:
    return "abs";
  case 0x2B:
    return "peek";
  case 0x2C:
    return "in";
  case 0x2D:
    return "usr-no";
  case 0x2E:
    return "str$";
  case 0x2F:
    return "chr$";
  case 0x30:
    return "not";
  case 0x31:
    return "duplicate";
  case 0x32:
    return "n-mod-m";
  case 0x33:
    return "jump";
  case 0x34:
    return "stk-data";
  case 0x35:
    return "dec-jr-nz";
  case 0x36:
    return "less-0";
  case 0x37:
    return "greater-0";
  case 0x38:
    return "end-calc";
  case 0x39:
    return "get-argt";
  case 0x3A:
    return "truncate";
  case 0x3B:
    return "fp-calc-2";
  case 0x3C:
    return "e-to-fp";
  case 0x3D:
    return "re-stack";
  case 0x3E:
    return "series-06";
  case 0x3F:
    return "stk-zero";
  case 0x40:
    return "st-mem-0";
  case 0x41:
    return "get-mem-0";
  case 0x92:
    return "delete";
  case 0xA0:
    return "stk-zero";
  case 0xA1:
    return "stk-one";
  case 0xA2:
    return "stk-half";
  case 0xA4:
    return "stk-ten";
  case 0xC0:
    return "st-mem-0";
  case 0xC1:
    return "st-mem-1";
  case 0xC3:
    return "st-mem-3";
  case 0xC4:
    return "st-mem-4";
  case 0xC5:
    return "st-mem-5";
  case 0xE0:
    return "get-mem-0";
  case 0xE1:
    return "get-mem-1";
  case 0xE2:
    return "get-mem-2";
  case 0xEF:
    return "exponent +7F"; //4 bytes arg
  default:
    return "unkn_fp_command";
  }
}


