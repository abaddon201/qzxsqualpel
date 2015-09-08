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

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return IDisassemblerCore::inst()->getMemoryByte(addr);
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
      //jump to (HL) or (IX) or (IY)
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

DisassemblerCoreZX::DisassemblerCoreZX(IGUIUpdater* updater)
  : IDisassemblerCore{updater, this} {
  ///@bug zx 128 have multiple segments 8k size each
  _memory->createSegment(0, 0xFFFF);
}

void DisassemblerCoreZX::init() {
}

void DisassemblerCoreZX::loadGuessFile(const std::string &fname) {
  std::ifstream f(fname);
  while (!f.eof()) {
    unsigned long long seg, off;
    std::string nm;
    f >> std::hex >> seg;
    f.ignore(1);
    f >> std::hex >> off;
    f >> nm;
    if (!nm.empty()) {
      Addr a(off, seg);
      _known_labels[a]=std::make_shared<Label>(a, nm);
    }
  }
}

int DisassemblerCoreZX::disassembleInstruction(const Addr &addr, std::shared_ptr<Chunk> &out_chunk) {
  char tbuff[128];
  size_t len;
  if (addr>=_memory->getMaxAddr()) {
    std::cerr<<"address out of range"<<addr.toString();
    return 0;
  }
  debugger_disassemble( tbuff, 128, &len, addr.offset() );

  if (len) {
    std::string buff{tbuff};
    //initialize chunk
    std::shared_ptr<Chunk> chunk_i = _chunks.getChunk(addr);
    if (chunk_i == nullptr) {
      std::cout<<"no instruction here: "<<addr.toString()<<std::endl;
      return 0;
    }
    if (!chunk_i->isEmpty())  {
      std::cout<<"allready parsed: "<< addr.toString() <<std::endl;
      return 0;
    }
    std::shared_ptr<Chunk> old_chunk = _chunks.getChunk(addr);
    _chunks.removeChunk(addr);
    std::shared_ptr<Chunk> target_chunk;
    if (addr.compare(0)) {
      //parsing current addr
      target_chunk=_chunks.createChunk(addr, Chunk::Type::CODE);
    } else {
      target_chunk=_chunks.getChunkContains(addr-1);
      if (target_chunk==nullptr) {
        std::cout<<"No target for disassemble"<<std::endl;
        target_chunk=_chunks.createChunk(addr, Chunk::Type::CODE);
      }
      //appending to prev. parsed block
      if (target_chunk->type()!=Chunk::Type::CODE) {
        std::cout<<"Not code previous chunk"<<std::endl;
        //parsing current addr
        target_chunk=_chunks.createChunk(addr, Chunk::Type::CODE);
      }
    }
    std::cout<<"addr="<<addr.toString()<<"command=" << buff <<"len=" << len<<std::endl;
    ///@bug must be in segment range check... think about it
    if (addr+len>=_memory->getMaxAddr()) {
      std::cout<<"instruction out of mem block"<<std::endl;
      _chunks.addChunk(addr, old_chunk);
      return -3;
    }
    if (len>1) {
      for (size_t i=1; i<len; i++) {
        std::shared_ptr<Chunk> ch=_chunks.getChunk(addr+i);
        if (ch==0) {
          std::cout<<"Instrunction longer than unparsed block"<<std::endl;
          //m_Chunks.removeChunk(addr);
          _chunks.addChunk(addr, old_chunk);
          return -4;
        }
        if (ch->type()!=Chunk::Type::UNPARSED) {
          std::cout<<"Instrunction longer than unparsed block2"<<std::endl;
          //m_Chunks.removeChunk(addr);
          _chunks.addChunk(addr, old_chunk);
          return -4;
        }
      }
      for (size_t i=1; i<len; i++) {
        _chunks.removeChunk(addr+i);
      }
    }
    Command cmd;
    cmd.addr = addr;
    cmd.len+=len;
    parseCommand(buff, cmd);
    target_chunk->appendCommand(cmd);
    std::cout<<"cmd appended"<<std::endl;
    len = postProcessChunk(target_chunk, len);
    out_chunk = target_chunk;
  }
  return len;
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

void DisassemblerCoreZX::disassembleBlock(const Addr &st_addr) {
  int res=0;
  Addr addr = st_addr;
  std::cout<<"disassembleBlock(): addr"<< addr.toString()<<std::endl;
  do {
    std::shared_ptr<Chunk> chunk;
    res=disassembleInstruction(addr, chunk);
    if (res==0) {
      //end of block
      break;
    }
    if (res<0) {
      //parse error
      return;
    }
    Addr jump_addr;
    //std::shared_ptr<CChunk> chunk=m_Chunks.getChunkContains(addr);
    if (chunk==nullptr) {
      std::cout<<"No chunk after disassemble instruction, addr:"<<addr.toString()<<std::endl;
      return;
    }
    switch (getLastCmdJumpType(chunk, jump_addr)) {
    case IDisassemblerCore::Type::JT_CALL:
      //call
      std::cout<<"call: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString()<<std::endl;
      std::cout<<"st_addr="<<st_addr.toString()<<std::endl;
      chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::CALL));
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_COND_JUMP:
      //conditional jump
      std::cout<<"cond jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString()<<std::endl;
      chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::JUMP));
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_JUMP:
      std::cout<<"jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString()<<std::endl;
      chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::JUMP));
      res=0;
      break;
    case IDisassemblerCore::Type::JT_COND_RET:
      //conditional ret
      std::cout<<"cond_ret: addr=" <<addr.toString()<<std::endl;
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_RET:
      std::cout<<"ret: addr=" <<addr.toString()<<std::endl;
      res=0;
      break;
    case IDisassemblerCore::Type::JT_NONE:
      addr+=res;
      break;
    }
  } while (res);
  std::cout<<"finished chunk:st_addr="<<st_addr.toString()<<" m_chunks.count="<<_chunks.count()<<std::endl;
}

bool DisassemblerCoreZX::labelPresent(const Addr &addr) const {
  return _labels.find(addr)!=_labels.end();
}

std::shared_ptr<Chunk> DisassemblerCoreZX::createChunk(const Addr &addr, Chunk::Type type) {
  return _chunks.createChunk(addr, type);
}

std::shared_ptr<Label> DisassemblerCoreZX::findKnownLabel(const Addr &addr) {
  auto it = _known_labels.find(addr);
  if (it != _known_labels.end()) {
    return it->second;
  }
  return nullptr;
}

std::shared_ptr<Label> DisassemblerCoreZX::makeJump(const Addr &from_addr, const Addr &jump_addr, Reference::Type ref_type) {
  disassembleBlock(jump_addr);
  std::shared_ptr<Chunk> jmp_chunk=_chunks.getChunk(jump_addr);
  if (jmp_chunk == nullptr) {
    std::cout<<"Split chunk at jump"<<std::endl;
    // split target chunk
    std::shared_ptr<Chunk> near_chunk=_chunks.getChunkContains(jump_addr);
    if (near_chunk==0) {
      std::cout<<"Fatal error on split: No target chunk"<<std::endl;
      return nullptr;
    }
    std::cout<<"near_chunk:addr"<<near_chunk->addr().toString()<<std::endl;
    jmp_chunk=near_chunk->splitAt(jump_addr);
    if (jmp_chunk==0) {
      std::cout<<"Split impossible"<<std::endl;
      return nullptr;
    }
  }
  jmp_chunk->addCrossRef(from_addr, ref_type);
  std::shared_ptr<Label> lbl = findKnownLabel(jump_addr);
  if (jmp_chunk->label() == nullptr) {
    lbl=jmp_chunk->setLabel(lbl, ref_type);
  } else {
    lbl=jmp_chunk->label();
  }
  if (!labelPresent(jump_addr)) {
    //CLabel label(jump_addr, lbl);
    _labels[jump_addr]=lbl;
  }
  return lbl;
}

void DisassemblerCoreZX::setRawMemory(unsigned char* buf, size_t size) {
  _memory->getSegment(0)->fill(buf, size);
}

void DisassemblerCoreZX::initialParse() {
  _chunks.clear();
  for (unsigned long long i=0; i<_memory->wholeSize(); ++i) {
    std::shared_ptr<Chunk> chunk=_chunks.createChunk(i, Chunk::Type::UNPARSED);
    Byte byte=_memory->getByte(i);
    Command cmd;
    cmd.command="db";
    cmd.addr = i;
    cmd.len=1;
    cmd.arg1=std::make_shared<ArgDefault>(byte.toString());
    chunk->appendCommand(cmd);
  }
  updater->updateWidgets();
}

