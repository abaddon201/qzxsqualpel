#include "i_disassembler_core.h"

#include "disassemble.h"

#include <memory>
#include <fstream>

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return (unsigned char)dasm::core::DisassemblerCore::inst()->getMemoryByte(addr);
}

namespace dasm {
namespace core {

void DisassemblerCore::loadGuessFile(const std::string& fname) {
  if (_auto_commenter) {
    _auto_commenter->loadGuessFile(fname);
  }
}

bool DisassemblerCore::labelPresent(const memory::Addr& addr) const {
  return _labels.find(addr) != _labels.end();
}

void DisassemblerCore::setRawMemory(unsigned char* buf, size_t size) {
  _memory->getSegment(0)->fill(buf, size);
}

void DisassemblerCore::initialParse() {
  _chunks.clear();
  Command cmd;
  for (unsigned long long i = 0; i < _memory->wholeSize(); ++i) {
    std::shared_ptr<Chunk> chunk = _chunks.createChunk(i, Chunk::Type::UNPARSED);
    Byte byte = _memory->getByte(i);
    cmd.command = "db";
    cmd.addr = i;
    cmd.len = 1;
    cmd.arg1 = std::make_shared<ArgDefault>(byte.toString());
    chunk->appendCommand(cmd);
  }
  updater->updateWidgets();
}

size_t DisassemblerCore::disassembleInstruction(const memory::Addr& addr, std::shared_ptr<Chunk>& out_chunk) {
  size_t len = 0;
  if (addr >= _memory->getMaxAddr()) {
    std::cerr << "address out of range" << addr.toString();
    return 0;
  }
  std::string buff = disassembleInstructionInt(addr, len);

  if (len) {
    if (addr + len >= _memory->getMaxAddr()) {
      std::cout << "instruction out of mem block" << std::endl;
      return -3;
    }
    //check address can be disassembled
    std::shared_ptr<Chunk> chunk_i = _chunks.getChunk(addr);
    if (chunk_i == nullptr) {
      // address is not loaded
      std::cout << "no instruction here: " << addr.toString() << std::endl;
      return 0;
    }
    if (!chunk_i->isEmpty()) {
      // address allready disassembled
      std::cout << "allready parsed: " << addr.toString() << std::endl;
      return 0;
    }
    ///@bug must be in segment range check... think about it
    if (len > 1) {
      // check that all bytes for the command are not parsed, if so, remove their chunks, if not, don't do anything
      for (size_t i = 1; i < len; i++) {
        auto ch = _chunks.getChunk(addr + i);
        if ((ch == nullptr) || (ch->type() != Chunk::Type::UNPARSED)) {
          std::cout << "Instrunction longer than unparsed block" << std::endl;
          return -4;
        }
      }
    }
    // found not disassembled chunk, remove it (we will replace it with code chunk)
    for (size_t i = 0; i < len; i++) {
      _chunks.removeChunk(addr + i);
    }
    std::shared_ptr<Chunk> target_chunk;
    if (addr.compare(0)) {
      //parse from memory start, there are no previous chunks present, so just say that it's now a code chunk
      target_chunk = _chunks.createChunk(addr, Chunk::Type::CODE);
    } else {
      // not at start of memory, possibly we have some code chunk before this address, check it
      target_chunk = _chunks.getChunkContains(addr - 1);
      LabelPtr klbl{ nullptr };
      if (_auto_commenter) {
        klbl = _auto_commenter->getLabelForAddr(addr);
      }
      if ((target_chunk == nullptr) || (target_chunk->type() != Chunk::Type::CODE) || (nullptr != klbl)) {
        // no code chunk before this address, create new one
        // or we found address with known label, start new chunk from it
        std::cout << "No CODE chunk before this address" << std::endl;
        target_chunk = _chunks.createChunk(addr, Chunk::Type::CODE);
        if (nullptr != klbl) {
          target_chunk->setLabel(klbl->name);
        }
      }
    }
    std::cout << "addr=" << addr.toString() << "command=" << buff << "len=" << len << std::endl;

    Command cmd;
    cmd.addr = addr;
    cmd.len += len;
    parseCommand(buff, cmd);
    target_chunk->appendCommand(cmd);
    std::cout << "cmd appended" << std::endl;
    len = postProcessChunk(target_chunk, len);
    out_chunk = target_chunk;
  }
  return len;
}

void DisassemblerCore::disassembleBlock(const memory::Addr& st_addr) {
  size_t res = 0;
  memory::Addr addr = st_addr;
  std::cout << "disassembleBlock(): addr" << addr.toString() << std::endl;
  do {
    std::shared_ptr<Chunk> chunk;
    res = disassembleInstruction(addr, chunk);
    if (res == 0) {
      //end of block
      break;
    }
    if (res < 0) {
      //parse error
      return;
    }
    memory::Addr jump_addr;
    //std::shared_ptr<CChunk> chunk=m_Chunks.getChunkContains(addr);
    if (chunk == nullptr) {
      std::cout << "No chunk after disassemble instruction, addr:" << addr.toString() << std::endl;
      return;
    }
    switch (getLastCmdJumpType(chunk, jump_addr)) {
      case JumpType::JT_CALL:
        //call
        std::cout << "call: addr=" << addr.toString() << "to_addr" << jump_addr.toString() << std::endl;
        std::cout << "st_addr=" << st_addr.toString() << std::endl;
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, memory::Reference::Type::CALL));
        addr += res;
        break;
      case JumpType::JT_COND_JUMP:
        //conditional jump
        std::cout << "cond jump: addr=" << addr.toString() << "to_addr" << jump_addr.toString() << std::endl;
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, memory::Reference::Type::JUMP));
        addr += res;
        break;
      case JumpType::JT_JUMP:
        std::cout << "jump: addr=" << addr.toString() << "to_addr" << jump_addr.toString() << std::endl;
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, memory::Reference::Type::JUMP));
        res = 0;
        break;
      case JumpType::JT_COND_RET:
        //conditional ret
        std::cout << "cond_ret: addr=" << addr.toString() << std::endl;
        addr += res;
        break;
      case JumpType::JT_RET:
        std::cout << "ret: addr=" << addr.toString() << std::endl;
        res = 0;
        break;
      case JumpType::JT_NONE:
        addr += res;
        break;
    }
    auto cmd = chunk->lastCommand();
    if (_auto_commenter) {
      _auto_commenter->commentCommand(cmd);
    }
  } while (res);
  std::cout << "finished chunk:st_addr=" << st_addr.toString() << " m_chunks.count=" << _chunks.count() << std::endl;
}

std::shared_ptr<Label>
DisassemblerCore::makeJump(const memory::Addr& from_addr, const memory::Addr& jump_addr, memory::Reference::Type ref_type) {
  disassembleBlock(jump_addr);
  std::shared_ptr<Chunk> jmp_chunk = _chunks.getChunk(jump_addr);
  if (jmp_chunk == nullptr) {
    std::cout << "Split chunk at jump" << std::endl;
    // split target chunk
    std::shared_ptr<Chunk> near_chunk = _chunks.getChunkContains(jump_addr);
    if (near_chunk == 0) {
      std::cout << "Fatal error on split: No target chunk" << std::endl;
      return nullptr;
    }
    std::cout << "near_chunk:addr" << near_chunk->addr().toString() << std::endl;
    jmp_chunk = near_chunk->splitAt(jump_addr);
    if (jmp_chunk == 0) {
      std::cout << "Split impossible" << std::endl;
      return nullptr;
    }
  }
  jmp_chunk->addCrossRef(from_addr, ref_type);
  std::shared_ptr<Label> lbl{ nullptr };
  if (_auto_commenter) {
    lbl = _auto_commenter->getLabelForAddr(jump_addr);
  }
  if (jmp_chunk->label() == nullptr) {
    lbl = jmp_chunk->setLabel(lbl, ref_type);
  } else {
    lbl = jmp_chunk->label();
  }
  if (!labelPresent(jump_addr)) {
    //CLabel label(jump_addr, lbl);
    _labels[jump_addr] = lbl;
  }
  return lbl;
}

JumpCmd DisassemblerCore::command2code(const std::string& cmd) const {
  if (cmd == "CALL") {
    return JumpCmd::CMD_CALL;
  } else if (cmd == "RST") {
    return JumpCmd::CMD_RST;
  } else if (cmd == "RET") {
    return JumpCmd::CMD_RET;
  } else if (cmd == "RETI") {
    return JumpCmd::CMD_RETI;
  } else if (cmd == "RETN") {
    return JumpCmd::CMD_RETN;
  } else if (cmd == "JP") {
    return JumpCmd::CMD_JP;
  } else if (cmd == "JR") {
    return JumpCmd::CMD_JR;
  } else {
    return JumpCmd::CMD_NONE;
  }
}

void DisassemblerCore::parseCommand(std::string& src, Command& out_command) {
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

std::string DisassemblerCore::disassembleInstructionInt(const memory::Addr& addr, size_t& len) {
  char tbuff[128];
  debugger_disassemble(tbuff, 128, &len, (libspectrum_word)addr.offset());
  return std::string(tbuff);
}

JumpType DisassemblerCore::getLastCmdJumpType(std::shared_ptr<Chunk> chunk, memory::Addr& jump_addr) {
  ///@bug rst 28 not last command in the chunk
  Command& cmd = chunk->lastCommand();
  if ((cmd.command_code == JumpCmd::CMD_CALL) || (cmd.command_code == JumpCmd::CMD_RST)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_CALL;
  }
  if (((cmd.command_code == JumpCmd::CMD_JR) || (cmd.command_code == JumpCmd::CMD_JP)) && (cmd.arg2 == nullptr)) {
    const std::string& arg1 = cmd.arg1->toString();
    if (arg1[0] == '"' || contains(arg1, "IX") || contains(arg1, "IY") || contains(arg1, "HL")) {
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return dasm::core::JumpType::JT_RET;
    }
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_JUMP;
  }
  if (((cmd.command_code == JumpCmd::CMD_JR) || (cmd.command_code == JumpCmd::CMD_JP)) && (cmd.arg2 != nullptr)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_COND_JUMP;
  }
  if ((cmd.command_code == JumpCmd::CMD_RET) && (cmd.arg1 != nullptr)) {
    return dasm::core::JumpType::JT_COND_RET;
  }
  if ((cmd.command_code == JumpCmd::CMD_RET) && (cmd.arg1 == nullptr)) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == JumpCmd::CMD_RETI) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == JumpCmd::CMD_RETN) {
    return dasm::core::JumpType::JT_RET;
  }
  return dasm::core::JumpType::JT_NONE;
}


size_t DisassemblerCore::postProcessChunk(std::shared_ptr<core::Chunk> chunk, size_t len) {
  auto cmd = chunk->lastCommand();
  if ((cmd.command == "RST") && (cmd.arg1->toString() == "28")) {
    return processRST28(chunk, len, cmd.addr);
  } else if (cmd.isLDICmd()) {
    //findAndMarkDEandHL();
    return len;
  } else if (cmd.command == "LD") {
    //check if it's accessed to memory
    std::string& arg1str = cmd.arg1->toString();
    if (arg1str.find("(") != arg1str.npos) {
      if (Register16::getRegister(arg1str) != Register16::None) {
        //need to find register fill
      } else {
        //it's memory address need to create memory label and replace arg with it
      }
    } else {
      std::string& arg2str = cmd.arg2->toString();
      if (arg2str.find("(") != arg2str.npos) {
        if (Register16::getRegister(arg2str) != Register16::None) {
          //need to find register fill
        } else {
          //it's memory address need to create memory label and replace arg with it
        }
      }
    }
  }
  return len;
}

size_t DisassemblerCore::processRST28(std::shared_ptr<core::Chunk> chunk, size_t len, const memory::Addr& addr) {
  /// RST 28 Ќужно анализировать особо... после RST располагаетс€ набор инструкций дл€ вычислений (bcalc)
  /// ѕример:
  /// 2E24 PF-SMALL RST 0028, FP-CALC
  ///               DEFB +E2,get-mem-2
  ///               DEFB +38,end-calc
  /// —обственно, стоит сделать умный анализ этого RST
  /// ¬ помощь книга: http://dac.escet.urjc.es/~csanchez/pfcs/zxspectrum/CompleteSpectrumROMDisassemblyThe.pdf
  /// (есть опечатки, например, 2dae, написано, что там 92, а на самом деле - 02
  /// http://zxpress.ru/book_articles.php?id=1099
  /// http://zxpress.ru/book_articles.php?id=1150
  /// http://zxpress.ru/book_articles.php?id=1051
  ///
  /// @bug: ѕереходы могут осуществл€тьс€ через куски нормального года, что приводит к возникновению "рваных" цепей
  /// пример такого бага: 2DE3. “ам осуществ€етс€ сери€ переходов, цели которых наход€тс€ после блока нормального ассемблера
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

std::string DisassemblerCore::getRST28AutoComment(unsigned char b, int& args_count) {
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
}
