//
// C++ Implementation: CDisassemblerCoreZX
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "core/debug_printers.h"
#include "core/utils.h"
#include "disassemble.h"
#include "memory.h"
#include "disassembler_core_ZX.h"

#include <vector>
#include <string>
#include <fstream>

DisassemblerCoreZX::DisassemblerCoreZX(IGUIUpdater* updater)
  : IDisassemblerCore{updater, this} {
  ///@bug zx 128 have multiple segments 8k size each
  _memory->createSegment(0, 0xFFFF);
}

void DisassemblerCoreZX::init() {
}

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return IDisassemblerCore::inst()->getMemoryByte(addr);
}

std::string DisassemblerCoreZX::disassembleInstructionInt(const Addr &addr, size_t len) {
  char tbuff[128];
  debugger_disassemble( tbuff, 128, &len, addr.offset() );
  return std::string(tbuff);
}

int DisassemblerCoreZX::command2code(const std::string &cmd) const {
  if (cmd == "CALL") {
    return CMD_CALL;
  } else if (cmd == "RST") {
    return CMD_RST;
  } else if (cmd == "RET") {
    return CMD_RET;
  } else if (cmd == "RETI") {
    return CMD_RETI;
  } else if (cmd == "RETN") {
    return CMD_RETN;
  } else if (cmd == "JP") {
    return CMD_JP;
  } else if (cmd == "JR") {
    return CMD_JR;
  } else {
    return CMD_NONE;
  }
}

IDisassemblerCore::Type DisassemblerCoreZX::getLastCmdJumpType(std::shared_ptr<Chunk> chunk, Addr &jump_addr) {
  ///@bug rst 28 not last command in the chunk
  Command &cmd=chunk->lastCommand();
  if ((cmd.command_code==CMD_CALL) || (cmd.command_code==CMD_RST)) {
    jump_addr=cmd.getJmpAddrFromString();
    return Type::JT_CALL;
  }
  if (((cmd.command_code==CMD_JR) || (cmd.command_code==CMD_JP)) && (cmd.arg2==nullptr)) {
    const std::string &arg1 = cmd.arg1->toString();
    if (arg1[0]=='"' || contains(arg1, "IX") || contains(arg1, "IY") || contains(arg1, "HL")) {
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return Type::JT_RET;
    }
    jump_addr=cmd.getJmpAddrFromString();
    return Type::JT_JUMP;
  }
  if (((cmd.command_code==CMD_JR) || (cmd.command_code==CMD_JP)) && (cmd.arg2!=nullptr)) {
    jump_addr=cmd.getJmpAddrFromString();
    return Type::JT_COND_JUMP;
  }
  if ((cmd.command_code==CMD_RET) && (cmd.arg1 != nullptr)) {
    return Type::JT_COND_RET;
  }
  if ((cmd.command_code==CMD_RET) && (cmd.arg1 == nullptr)) {
    return Type::JT_RET;
  }
  if (cmd.command_code==CMD_RETI) {
    return Type::JT_RET;
  }
  if (cmd.command_code==CMD_RETN) {
    return Type::JT_RET;
  }
  return Type::JT_NONE;
}

void DisassemblerCoreZX::parseCommand(std::string &src, Command &out_command) {
  std::vector<std::string> strlist=split(src,' ');
  out_command.command=strlist[0];
  out_command.command_code = command2code(out_command.command);
  if (strlist.size()>1) {
    //has args
    std::vector<std::string> args=split(strlist[1], ',');
    out_command.arg1=std::make_shared<ArgDefault>(args[0]);
    if (args.size()==2) {
      out_command.arg2=std::make_shared<ArgDefault>(args[1]);
    }
    autoCommentCommand(out_command);
  }
}

int DisassemblerCoreZX::postProcessChunk(std::shared_ptr<Chunk> chunk, int len) {
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
  auto cmd = chunk->lastCommand();
  if ((cmd.command=="RST") && (cmd.arg1->toString()=="28")) {
    Addr a=cmd.addr+1;

    Byte b;
    Command c;
    int args_cnt;
    try {
      while ((b=_memory->getByte(a))!=0x38) {
        c.addr=a;
        c.command="DB";
        c.arg1=std::make_shared<ArgDefault>(b.toString());
        c.auto_comment = getRST28AutoComment(b, args_cnt);
        c.len=1;

        _chunks.removeChunk(a);
        chunk->appendCommand(c);
        len++;
        ++a;
        if (args_cnt) {
          c.addr=a;
          c.command="DB";
          b=_memory->getByte(a);
          c.arg1=std::make_shared<ArgDefault>(b.toString());
          c.auto_comment = "dest_addr: "+Addr(a+int{(signed char)b}).toString();
          c.len=1;

          _chunks.removeChunk(a);
          chunk->appendCommand(c);
          len++;
          ++a;
        }
      }
      c.addr=a;
      c.command="DB";
      c.arg1=std::make_shared<ArgDefault>(b.toString());
      c.len=1;
      c.auto_comment = getRST28AutoComment(b, args_cnt);

      _chunks.removeChunk(a);
      chunk->appendCommand(c);
      len++;
    } catch (std::out_of_range &) {
      std::cout<<"finished due address exceeds"<<std::endl;
    }
  }
  return len;
}
