#include "i_disassembler_core.h"

#include <memory>
#include <fstream>

namespace dasm {
namespace core {
IDisassemblerCore* IDisassemblerCore::_inst = nullptr;

void IDisassemblerCore::loadGuessFile(const std::string& fname) {
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
      _known_labels[a] = std::make_shared<Label>(a, nm);
    }
  }
}

bool IDisassemblerCore::labelPresent(const Addr& addr) const {
  return _labels.find(addr) != _labels.end();
}

std::shared_ptr<Label> IDisassemblerCore::findKnownLabel(const Addr& addr) {
  auto it = _known_labels.find(addr);
  if (it != _known_labels.end()) {
    return it->second;
  }
  return nullptr;
}

void IDisassemblerCore::setRawMemory(unsigned char* buf, size_t size) {
  _memory->getSegment(0)->fill(buf, size);
}

void IDisassemblerCore::initialParse() {
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

int IDisassemblerCore::disassembleInstruction(const Addr& addr, std::shared_ptr<Chunk>& out_chunk) {
  size_t len = 0;
  if (addr >= _memory->getMaxAddr()) {
    std::cerr << "address out of range" << addr.toString();
    return 0;
  }
  std::string buff = disassembleInstructionInt(addr, len);

  if (len) {
    //initialize chunk
    std::shared_ptr<Chunk> chunk_i = _chunks.getChunk(addr);
    if (chunk_i == nullptr) {
      std::cout << "no instruction here: " << addr.toString() << std::endl;
      return 0;
    }
    if (!chunk_i->isEmpty()) {
      std::cout << "allready parsed: " << addr.toString() << std::endl;
      return 0;
    }
    std::shared_ptr<Chunk> old_chunk = _chunks.getChunk(addr);
    _chunks.removeChunk(addr);
    std::shared_ptr<Chunk> target_chunk;
    if (addr.compare(0)) {
      //parsing current addr
      target_chunk = _chunks.createChunk(addr, Chunk::Type::CODE);
    } else {
      target_chunk = _chunks.getChunkContains(addr - 1);
      if (target_chunk == nullptr) {
        std::cout << "No target for disassemble" << std::endl;
        target_chunk = _chunks.createChunk(addr, Chunk::Type::CODE);
      }
      //appending to prev. parsed block
      if (target_chunk->type() != Chunk::Type::CODE) {
        std::cout << "Not code previous chunk" << std::endl;
        //parsing current addr
        target_chunk = _chunks.createChunk(addr, Chunk::Type::CODE);
      }
    }
    std::cout << "addr=" << addr.toString() << "command=" << buff << "len=" << len << std::endl;
    ///@bug must be in segment range check... think about it
    if (addr + len >= _memory->getMaxAddr()) {
      std::cout << "instruction out of mem block" << std::endl;
      _chunks.addChunk(addr, old_chunk);
      return -3;
    }
    if (len > 1) {
      for (size_t i = 1; i < len; i++) {
        auto ch = _chunks.getChunk(addr + i);
        if (ch == nullptr) {
          std::cout << "Instrunction longer than unparsed block" << std::endl;
          //m_Chunks.removeChunk(addr);
          _chunks.addChunk(addr, old_chunk);
          return -4;
        }
        if (ch->type() != Chunk::Type::UNPARSED) {
          std::cout << "Instrunction longer than unparsed block2" << std::endl;
          //m_Chunks.removeChunk(addr);
          _chunks.addChunk(addr, old_chunk);
          return -4;
        }
      }
      for (size_t i = 1; i < len; i++) {
        _chunks.removeChunk(addr + i);
      }
    }
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

void IDisassemblerCore::disassembleBlock(const Addr& st_addr) {
  int res = 0;
  Addr addr = st_addr;
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
    Addr jump_addr;
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
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::CALL));
        addr += res;
        break;
      case JumpType::JT_COND_JUMP:
        //conditional jump
        std::cout << "cond jump: addr=" << addr.toString() << "to_addr" << jump_addr.toString() << std::endl;
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::JUMP));
        addr += res;
        break;
      case JumpType::JT_JUMP:
        std::cout << "jump: addr=" << addr.toString() << "to_addr" << jump_addr.toString() << std::endl;
        chunk->lastCommand().setJmpAddr(makeJump(addr, jump_addr, Reference::Type::JUMP));
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
IDisassemblerCore::makeJump(const Addr& from_addr, const Addr& jump_addr, Reference::Type ref_type) {
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
  std::shared_ptr<Label> lbl = findKnownLabel(jump_addr);
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

}
}
