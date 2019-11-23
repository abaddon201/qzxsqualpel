//
// C++ Implementation: CDisassemblerCoreZX
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2019
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "core/debug_printers.h"
#include "utils/utils.h"
#include "core/disassemble.h"
#include "memory.h"
#include "disassembler_core_ZX.h"
#include "postprocessors/autocommenter.h"
#include "core/jump_type.h"

#include <vector>
#include <string>
#include <fstream>

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return (unsigned char)dasm::core::IDisassemblerCore::inst()->getMemoryByte(addr);
}

namespace dasm {
DisassemblerCoreZX::DisassemblerCoreZX(IGUIUpdater* updater)
  : IDisassemblerCore{ updater, this, std::make_shared<postprocessors::AutoCommenter>() } {
  ///@bug zx 128 have multiple segments 8k size each
  _memory->createSegment(0, 0xFFFF);
}

void DisassemblerCoreZX::init() {}

std::string DisassemblerCoreZX::disassembleInstructionInt(const memory::Addr& addr, size_t& len) {
  char tbuff[128];
  debugger_disassemble(tbuff, 128, &len, (libspectrum_word)addr.offset());
  return std::string(tbuff);
}

int DisassemblerCoreZX::command2code(const std::string& cmd) const {
  if (cmd == "CALL") {
    return (int)JumpCmd::CMD_CALL;
  } else if (cmd == "RST") {
    return (int)JumpCmd::CMD_RST;
  } else if (cmd == "RET") {
    return (int)JumpCmd::CMD_RET;
  } else if (cmd == "RETI") {
    return (int)JumpCmd::CMD_RETI;
  } else if (cmd == "RETN") {
    return (int)JumpCmd::CMD_RETN;
  } else if (cmd == "JP") {
    return (int)JumpCmd::CMD_JP;
  } else if (cmd == "JR") {
    return (int)JumpCmd::CMD_JR;
  } else {
    return (int)JumpCmd::CMD_NONE;
  }
}

core::JumpType DisassemblerCoreZX::getLastCmdJumpType(std::shared_ptr<core::Chunk> chunk, memory::Addr& jump_addr) {
  ///@bug rst 28 not last command in the chunk
  Command& cmd = chunk->lastCommand();
  if ((cmd.command_code == (int)JumpCmd::CMD_CALL) || (cmd.command_code == (int)JumpCmd::CMD_RST)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_CALL;
  }
  if (((cmd.command_code == (int)JumpCmd::CMD_JR) || (cmd.command_code == (int)JumpCmd::CMD_JP)) && (cmd.arg2 == nullptr)) {
    const std::string& arg1 = cmd.arg1->toString();
    if (arg1[0] == '"' || contains(arg1, "IX") || contains(arg1, "IY") || contains(arg1, "HL")) {
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return dasm::core::JumpType::JT_RET;
    }
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_JUMP;
  }
  if (((cmd.command_code == (int)JumpCmd::CMD_JR) || (cmd.command_code == (int)JumpCmd::CMD_JP)) && (cmd.arg2 != nullptr)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_COND_JUMP;
  }
  if ((cmd.command_code == (int)JumpCmd::CMD_RET) && (cmd.arg1 != nullptr)) {
    return dasm::core::JumpType::JT_COND_RET;
  }
  if ((cmd.command_code == (int)JumpCmd::CMD_RET) && (cmd.arg1 == nullptr)) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == (int)JumpCmd::CMD_RETI) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == (int)JumpCmd::CMD_RETN) {
    return dasm::core::JumpType::JT_RET;
  }
  return dasm::core::JumpType::JT_NONE;
}

void DisassemblerCoreZX::parseCommand(std::string& src, core::Command& out_command) {
  std::vector<std::string> strlist = split(src, ' ');
  out_command.command = strlist[0];
  out_command.command_code = command2code(out_command.command);
  if (strlist.size() > 1) {
    //has args
    std::vector<std::string> args = split(strlist[1], ',');
    out_command.arg1 = std::make_shared<ArgDefault>(args[0]);
    if (args.size() == 2) {
      out_command.arg2 = std::make_shared<ArgDefault>(args[1]);
    }
  }
}

size_t DisassemblerCoreZX::postProcessChunk(std::shared_ptr<core::Chunk> chunk, size_t len) {
  auto cmd = chunk->lastCommand();
  if ((cmd.command == "RST") && (cmd.arg1->toString() == "28")) {
    return processRST28(chunk, len, cmd.addr);
  } else if (isLDICmd(cmd)) {
    //findAndMarkDEandHL();
    return len;
  } else if (cmd.command == "LD") {
    //check if it's accessed to memory
    std::string& arg1str = cmd.arg1->toString();
    if (arg1str.find("(") != arg1str.npos) {
      if (getRegister(arg1str) != Register16::None) {
        //need to find register fill
      } else {
        //it's memory address need to create memory label and replace arg with it
      }
    } else {
      std::string& arg2str = cmd.arg2->toString();
      if (arg2str.find("(") != arg2str.npos) {
        if (getRegister(arg2str) != Register16::None) {
          //need to find register fill
        } else {
          //it's memory address need to create memory label and replace arg with it
        }
      }
    }
  }
  return len;
}

Register16 DisassemblerCoreZX::getRegister(const std::string& arg) {
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

bool DisassemblerCoreZX::isLDICmd(const core::Command& cmd) {
  return (cmd.command == "LDI") || (cmd.command == "LDIR") || (cmd.command == "LDD") || (cmd.command == "LDDR");
}

size_t DisassemblerCoreZX::processRST28(std::shared_ptr<core::Chunk> chunk, size_t len, const memory::Addr& addr) {
  /// RST 28 Нужно анализировать особо... после RST располагается набор инструкций для вычислений (bcalc)
  /// Пример:
  /// 2E24 PF-SMALL RST 0028, FP-CALC
  ///               DEFB +E2,get-mem-2
  ///               DEFB +38,end-calc
  /// Собственно, стоит сделать умный анализ этого RST
  /// В помощь книга: http://dac.escet.urjc.es/~csanchez/pfcs/zxspectrum/CompleteSpectrumROMDisassemblyThe.pdf
  /// (есть опечатки, например, 2dae, написано, что там 92, а на самом деле - 02
  /// http://zxpress.ru/book_articles.php?id=1099
  /// http://zxpress.ru/book_articles.php?id=1150
  /// http://zxpress.ru/book_articles.php?id=1051
  ///
  /// @bug: Переходы могут осуществляться через куски нормального года, что приводит к возникновению "рваных" цепей
  /// пример такого бага: 2DE3. Там осуществяется серия переходов, цели которых находятся после блока нормального ассемблера
  memory::Addr a = addr + 1;

  Byte b;
  Command c;
  int args_cnt;
  try {
    while ((unsigned char)(b = _memory->getByte(a)) != 0x38) {
      c.addr = a;
      c.command = "DB";
      c.arg1 = std::make_shared<ArgDefault>(b.toString());
      c.auto_comment = getRST28AutoComment((unsigned char)b, args_cnt);
      c.len = 1;

      _chunks.removeChunk(a);
      chunk->appendCommand(c);
      len++;
      ++a;
      if (args_cnt) {
        c.addr = a;
        c.command = "DB";
        b = _memory->getByte(a);
        c.arg1 = std::make_shared<ArgDefault>(b.toString());
        c.auto_comment = "dest_addr: " + memory::Addr(a + int{ (signed char)(unsigned char)b }).toString();
        c.len = 1;

        _chunks.removeChunk(a);
        chunk->appendCommand(c);
        len++;
        ++a;
      }
    }
    c.addr = a;
    c.command = "DB";
    c.arg1 = std::make_shared<ArgDefault>(b.toString());
    c.len = 1;
    c.auto_comment = getRST28AutoComment((unsigned char)b, args_cnt);

    _chunks.removeChunk(a);
    chunk->appendCommand(c);
    len++;
  } catch (std::out_of_range&) {
    std::cout << "finished due address exceeds" << std::endl;
  }
  return len;
}

std::string DisassemblerCoreZX::getRST28AutoComment(unsigned char b, int& args_count) {
  args_count = 0;
  switch (b) {
    case 0x00:
      args_count = 1;
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
      args_count = 1;
      return "jump";
    case 0x34:
      return "stk-data";
    case 0x35:
      args_count = 1;
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
    case 0x80:
      return "cheb-00";
    case 0x81:
      return "cheb-01";
    case 0x82:
      return "cheb-02";
    case 0x83:
      return "cheb-03";
    case 0x84:
      return "cheb-04";
    case 0x85:
      return "cheb-05";
    case 0x86:
      return "cheb-06";
    case 0x87:
      return "cheb-07";
    case 0x88:
      return "cheb-08";
    case 0x89:
      return "cheb-09";
    case 0x90:
      return "cheb-10";
    case 0x91:
      return "cheb-11";
    case 0x92:
      return "cheb-12";
    case 0x93:
      return "cheb-13";
    case 0x94:
      return "cheb-14";
    case 0x95:
      return "cheb-15";
    case 0x96:
      return "cheb-16";
    case 0x97:
      return "cheb-17";
    case 0x98:
      return "cheb-18";
    case 0x99:
      return "cheb-19";
    case 0xA0:
      return "stk-zero";
    case 0xA1:
      return "stk-one";
    case 0xA2:
      return "stk-half";
    case 0xA3:
      return "stk-pi/2";
    case 0xA4:
      return "stk-ten";
    case 0xC0:
      return "st-mem-00";
    case 0xC1:
      return "st-mem-01";
    case 0xC2:
      return "st-mem-02";
    case 0xC3:
      return "st-mem-03";
    case 0xC4:
      return "st-mem-04";
    case 0xC5:
      return "st-mem-05";
    case 0xC6:
      return "st-mem-06";
    case 0xC7:
      return "st-mem-07";
    case 0xC8:
      return "st-mem-08";
    case 0xC9:
      return "st-mem-09";
    case 0xCA:
      return "st-mem-0A";
    case 0xCB:
      return "st-mem-0B";
    case 0xCC:
      return "st-mem-0C";
    case 0xCD:
      return "st-mem-0D";
    case 0xCE:
      return "st-mem-0E";
    case 0xCF:
      return "st-mem-0F";
    case 0xD0:
      return "st-mem-10";
    case 0xD1:
      return "st-mem-11";
    case 0xD2:
      return "st-mem-12";
    case 0xD3:
      return "st-mem-13";
    case 0xD4:
      return "st-mem-14";
    case 0xD5:
      return "st-mem-15";
    case 0xD6:
      return "st-mem-16";
    case 0xD7:
      return "st-mem-17";
    case 0xD8:
      return "st-mem-18";
    case 0xD9:
      return "st-mem-19";
    case 0xDA:
      return "st-mem-1A";
    case 0xDB:
      return "st-mem-1B";
    case 0xDC:
      return "st-mem-1C";
    case 0xDD:
      return "st-mem-1D";
    case 0xDE:
      return "st-mem-1E";
    case 0xDF:
      return "st-mem-1F";
    case 0xE0:
      return "get-mem-00";
    case 0xE1:
      return "get-mem-01";
    case 0xE2:
      return "get-mem-02";
    case 0xE3:
      return "get-mem-03";
    case 0xE4:
      return "get-mem-04";
    case 0xE5:
      return "get-mem-05";
    case 0xE6:
      return "get-mem-06";
    case 0xE7:
      return "get-mem-07";
    case 0xE8:
      return "get-mem-08";
    case 0xE9:
      return "get-mem-09";
    case 0xEA:
      return "get-mem-0A";
    case 0xEB:
      return "get-mem-0B";
    case 0xEC:
      return "get-mem-0C";
    case 0xED:
      return "get-mem-0D";
    case 0xEE:
      return "get-mem-0E";
    case 0xEF:
      return "get-mem-0F";
    case 0xF0:
      return "get-mem-00";
    case 0xF1:
      return "get-mem-11";
    case 0xF2:
      return "get-mem-12";
    case 0xF3:
      return "get-mem-13";
    case 0xF4:
      return "get-mem-14";
    case 0xF5:
      return "get-mem-15";
    case 0xF6:
      return "get-mem-16";
    case 0xF7:
      return "get-mem-17";
    case 0xF8:
      return "get-mem-18";
    case 0xF9:
      return "get-mem-19";
    case 0xFA:
      return "get-mem-1A";
    case 0xFB:
      return "get-mem-1B";
    case 0xFC:
      return "get-mem-1C";
    case 0xFD:
      return "get-mem-1D";
    case 0xFE:
      return "get-mem-1E";
    case 0xFF:
      return "get-mem-1F";
      /** @bug в книге дизасм пзу так:  case 0xEF:
          args_count=4;
          return "exponent +7F"; //4 bytes arg
          */
    default:
      return "unkn_fp_command";
  }
}

}
