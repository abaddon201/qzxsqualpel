#include "disassembler_core.h"

#include "disassemble.h"
#include "postprocessors/rst28.h"

#include <memory>
#include <fstream>

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return (unsigned char)dasm::core::DisassemblerCore::inst().memory().getByte(addr);
}

namespace dasm {
namespace core {

void DisassemblerCore::init(IGUIUpdater* updater_) {
  updater = updater_;

  //_memory = std::make_unique<memory::Memory>();
  ///@bug zx 128 have multiple segments 8k size each
  _memory.createSegment(0, 0xFFFF);

  _auto_commenter = std::make_shared<postprocessors::AutoCommenter>();
  _postprocessors.push_back(std::make_shared<postprocessors::Rst28>());
};

void DisassemblerCore::loadGuessFile(const std::string& fname) {
  if (_auto_commenter) {
    _auto_commenter->loadGuessFile(fname);
  }
}

bool DisassemblerCore::labelPresent(const memory::Addr& addr) const {
  return _labels.find(addr) != _labels.end();
}

void DisassemblerCore::setRawMemory(unsigned char* buf, size_t size) {
  _memory.getSegment(0)->fill(buf, size);
}

void DisassemblerCore::initialParse() {
  _chunks.clear();
  Command cmd;
  for (unsigned long long i = 0; i < _memory.wholeSize(); ++i) {
    std::shared_ptr<Chunk> chunk = _chunks.createChunk(i, Chunk::Type::UNPARSED);
    Byte byte = _memory.getByte(i);
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
  if (addr >= _memory.getMaxAddr()) {
    std::cerr << "address out of range" << addr.toString();
    return 0;
  }
  std::string buff = disassembleInstructionInt(addr, len);

  if (len) {
    if (addr + len >= _memory.getMaxAddr()) {
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
    cmd.parse(buff);
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

std::string DisassemblerCore::disassembleInstructionInt(const memory::Addr& addr, size_t& len) {
  char tbuff[128];
  debugger_disassemble(tbuff, 128, &len, (libspectrum_word)addr.offset());
  return std::string(tbuff);
}

JumpType DisassemblerCore::getLastCmdJumpType(std::shared_ptr<Chunk> chunk, memory::Addr& jump_addr) {
  ///@bug rst 28 not last command in the chunk
  Command& cmd = chunk->lastCommand();
  if ((cmd.command_code == CmdCode::CMD_CALL) || (cmd.command_code == CmdCode::CMD_RST)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_CALL;
  }
  if (((cmd.command_code == CmdCode::CMD_JR) || (cmd.command_code == CmdCode::CMD_JP)) && (cmd.arg2 == nullptr)) {
    const std::string& arg1 = cmd.arg1->toString();
    if (arg1[0] == '"' || contains(arg1, "IX") || contains(arg1, "IY") || contains(arg1, "HL")) {
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return dasm::core::JumpType::JT_RET;
    }
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_JUMP;
  }
  if (((cmd.command_code == CmdCode::CMD_JR) || (cmd.command_code == CmdCode::CMD_JP)) && (cmd.arg2 != nullptr)) {
    jump_addr = cmd.getJmpAddrFromString();
    return dasm::core::JumpType::JT_COND_JUMP;
  }
  if ((cmd.command_code == CmdCode::CMD_RET) && (cmd.arg1 != nullptr)) {
    return dasm::core::JumpType::JT_COND_RET;
  }
  if ((cmd.command_code == CmdCode::CMD_RET) && (cmd.arg1 == nullptr)) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == CmdCode::CMD_RETI) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd.command_code == CmdCode::CMD_RETN) {
    return dasm::core::JumpType::JT_RET;
  }
  return dasm::core::JumpType::JT_NONE;
}


size_t DisassemblerCore::postProcessChunk(std::shared_ptr<core::Chunk> chunk, size_t len) {
  for (auto& p : _postprocessors) {
    if (p->checkPrecondition(chunk)) {
      len = p->process(chunk, len);
    }
  }
  /*if (cmd.isLDICmd()) {
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
  }*/
  return len;
}


}
}
