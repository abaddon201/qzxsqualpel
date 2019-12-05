#include "disassembler_core.h"

#include "disassemble.h"
#include "postprocessors/rst28.h"
#include "postprocessors/register_reference_scan.h"

#include <memory>
#include <fstream>

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return (unsigned char)dasm::core::DisassemblerCore::inst().memory().byte(addr);
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
  _postprocessors.push_back(std::make_shared<postprocessors::RegisterReferenceScan>());
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
  _memory.segment(0)->fill(buf, size);
}

void DisassemblerCore::initialParse() {
  _chunks.clear();
  for (unsigned long long i = 0; i < _memory.wholeSize(); ++i) {
    std::shared_ptr<Chunk> chunk = _chunks.createChunk(i, Chunk::Type::UNPARSED);
    CommandPtr cmd = std::make_shared<Command>();
    Byte byte = _memory.byte(i);
    //cmd.command = "db";
    cmd->command_code = CmdCode::NONE;
    cmd->addr = i;
    cmd->len = 1;
    cmd->setArg(0, std::make_shared<ArgDefault>(byte));
    chunk->appendCommand(cmd);
  }
  updater->updateWidgets();
}

size_t DisassemblerCore::disassembleInstruction(const memory::Addr& addr, std::shared_ptr<Chunk>& out_chunk) {
  size_t len = 0;
  if (addr >= _memory.maxAddr()) {
    std::cerr << "address out of range" << addr.toString();
    return 0;
  }
  std::string buff = disassembleInstructionInt(addr, len);

  if (len) {
    if (addr + len >= _memory.maxAddr()) {
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

    CommandPtr cmd = std::make_shared<Command>();
    cmd->addr = addr;
    cmd->len = len;
    cmd->parse(buff);
    if (_auto_commenter) {
      _auto_commenter->commentCommand(cmd);
    }
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
    switch (lastCmdJumpType(chunk, jump_addr)) {
      case JumpType::JT_CALL: {
        //call
        std::cout << "!!!! call: addr=" << addr.toString() << " to_addr " << jump_addr.toString() << std::endl;
        std::cout << "st_addr=" << st_addr.toString() << std::endl;
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::CALL);
        auto& ch = _chunks.getChunkContains(addr);
        ch->lastCommand()->setJmpAddr(lbl);
        addr += res;
        break;
      }
      case JumpType::JT_COND_JUMP: {
        //conditional jump
        std::cout << "!!!! cond jump: addr=" << addr.toString() << " to_addr " << jump_addr.toString() << std::endl;
        //auto& lastcmd = chunk->lastCommand();
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::JUMP);
        auto& ch = _chunks.getChunkContains(addr);
        ch->getCommand(addr)->setJmpAddr(lbl);
        ;
        addr += res;
        break;
      }
      case JumpType::JT_JUMP: {
        std::cout << "!!!! jump: addr=" << addr.toString() << " to_addr " << jump_addr.toString() << std::endl;
        auto& lbl = makeJump(addr, jump_addr, memory::Reference::Type::JUMP);
        auto& ch = _chunks.getChunkContains(addr);
        ch->lastCommand()->setJmpAddr(lbl);
        res = 0;
        break;
      }
      case JumpType::JT_COND_RET:
        //conditional ret
        std::cout << "!!!! cond_ret: addr=" << addr.toString() << std::endl;
        addr += res;
        break;
      case JumpType::JT_RET:
        std::cout << "!!!! ret: addr=" << addr.toString() << std::endl;
        res = 0;
        break;
      case JumpType::JT_NONE:
        addr += res;
        break;
    }
  } while (res);
  std::cout << "finished chunk:st_addr=" << st_addr.toString() << " m_chunks.count=" << _chunks.count() << std::endl;
}

/*
ABBY
0x0008, 0x000b LD HL, (word); LD (word), HL
0x0c90 LD (byte_5c8c), A
0x03d9 LD C, 3f
0x0b9b LD A, (byte_5c91)
0x0bdf AND 03
0x0be1 OR 58
0x0c0b LD H, 00
0x11a7 (back jump)
0x2e24 (rst 28)
0x1276 (index reg) (navigation bug)
0x2aa9 res 6,(IY+1)
0x1287 set 1, (IY+1)
0x12a2 LD (IY+31), 02
0x122e IM 1
0x11ce OUT fe, A
0x007d CP 21

0x0b52 SUB a5
0x0b56 ADD A,15
0x0cf4 DJNZ 0x0cf0
0x0bbd EX AF, AF'

0x5c72 DW ffff

0x1222 LD HL, (word_5cb2)
0x1225 LD (HL), 3e

0x120a LDDR

0x137c LD HL,5C44
0x137f BIT 7, (HL)

0x0daf LD HL, 0000 // brakes code at start
0x0dc9 LD HL, (word_5c51) //wrong label (not inited) 0x1615 - address of first initialization

0x0edf LD HL, byte_5b00
0x0ee7 LD (HL), A
*/
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
  return addCrossRef(jmp_chunk, from_addr, jump_addr, ref_type);
}

LabelPtr DisassemblerCore::addCrossRef(ChunkPtr chunk, const memory::Addr& from_addr, const memory::Addr& dst_addr, memory::Reference::Type ref_type) {
  chunk->addCrossRef(from_addr, ref_type);
  LabelPtr lbl{ nullptr };
  if (_auto_commenter) {
    lbl = _auto_commenter->getLabelForAddr(dst_addr);
  }
  if (chunk->label() == nullptr) {
    lbl = chunk->setLabel(lbl, ref_type);
  } else {
    lbl = chunk->label();
  }
  if (!labelPresent(dst_addr)) {
    //CLabel label(jump_addr, lbl);
    _labels[dst_addr] = lbl;
  }
  return lbl;
}

std::shared_ptr<Label>
DisassemblerCore::makeData(const memory::Addr& from_addr, const memory::Addr& data_addr, memory::Reference::Type ref_type) {
  auto data_chunk = _chunks.getChunk(data_addr);
  if (data_chunk == nullptr) {
    std::cout << "Split chunk at data" << std::endl;
    // split target chunk
    auto near_chunk = _chunks.getChunkContains(data_addr);
    if (near_chunk == 0) {
      std::cout << "Fatal error on split: No target chunk" << std::endl;
      return nullptr;
    }
    if ((near_chunk->type() == Chunk::Type::DATA_BYTE_ARRAY) || (near_chunk->type() == Chunk::Type::DATA_WORD_ARRAY)) {
      // here is an array
      // FIXME: here we must show offset for data in the array and check data access type
      return addCrossRef(near_chunk, from_addr, data_addr, ref_type);
    } else {
      // other cases, such as code (self modifying)
      std::cout << "Fatal error on split: here is a code or other chunk" << std::endl;
    }
    return nullptr;
  } else {
    if ((ref_type == memory::Reference::Type::WRITE_BYTE) ||
      (ref_type == memory::Reference::Type::READ_BYTE)) {
      if (data_chunk->type() == Chunk::Type::CODE) {
        return nullptr;
      }
      if (data_chunk->type() == Chunk::Type::UNPARSED) {
        _chunks.removeChunk(data_addr);
        data_chunk = _chunks.createChunk(data_addr, Chunk::Type::DATA_BYTE);
        Byte byte = _memory.byte(data_addr);
        CommandPtr cmd = std::make_shared<Command>();
        cmd->command_code = CmdCode::DB;
        cmd->addr = data_addr;
        cmd->len = 1;
        cmd->setArg(0, std::make_shared<ArgDefault>(byte));
        data_chunk->appendCommand(cmd);
      }
    } else if ((ref_type == memory::Reference::Type::WRITE_WORD) || (ref_type == memory::Reference::Type::READ_WORD)) {
      if (data_chunk->type() == Chunk::Type::CODE) {
        return nullptr;
      }
      if (data_chunk->type() == Chunk::Type::UNPARSED) {
        _chunks.removeChunk(data_addr);
        _chunks.removeChunk(data_addr + 1);
        data_chunk = _chunks.createChunk(data_addr, Chunk::Type::DATA_WORD);
        Byte bl = _memory.byte(data_addr);
        Byte bh = _memory.byte(data_addr + 1);
        CommandPtr cmd = std::make_shared<Command>();
        cmd->command_code = CmdCode::DW;
        cmd->addr = data_addr;
        cmd->len = 2;
        cmd->setArg(0, std::make_shared<ArgDefault>((((uint16_t)((uint8_t)bh)) << 8) | ((uint16_t)((uint8_t)bl)), ArgSize::Word, true));
        data_chunk->appendCommand(cmd);
      }
    }
    return addCrossRef(data_chunk, from_addr, data_addr, ref_type);
  }
}

void DisassemblerCore::makeArray(const memory::Addr& from_addr, int size, bool clearMem) {
  if (clearMem) {
    memory::Addr addr = from_addr;
    int sz = size;
    for (; sz != 0; sz--, ++addr) {
      _memory.setByte(addr, Byte(0));
    }
  }
  memory::Addr addr = from_addr;
  auto chunk = std::make_shared<Chunk>(from_addr, Chunk::Type::DATA_BYTE_ARRAY);
  CommandPtr cmd = std::make_shared<Command>();
  cmd->command_code = CmdCode::DB;
  cmd->addr = from_addr;
  cmd->len = size;
  auto arg = std::make_shared<ArgByteArray>(size);
  for (; size != 0; size--, ++addr) {
    _chunks.removeChunk(addr);
    Byte byte = _memory.byte(addr);
    arg->pushByte(byte);
  }
  cmd->setArg(0, arg);
  chunk->appendCommand(cmd);
  _chunks.addChunk(from_addr, chunk);
}

std::string DisassemblerCore::disassembleInstructionInt(const memory::Addr& addr, size_t& len) {
  char tbuff[128];
  debugger_disassemble(tbuff, 128, &len, (libspectrum_word)addr.offset());
  return std::string(tbuff);
}

JumpType DisassemblerCore::lastCmdJumpType(std::shared_ptr<Chunk> chunk, memory::Addr& jump_addr) {
  CommandPtr cmd = chunk->lastCommand();
  if ((cmd->command_code == CmdCode::CALL) || (cmd->command_code == CmdCode::RST)) {
    jump_addr = cmd->getJmpAddrFromString();
    return dasm::core::JumpType::JT_CALL;
  }
  if (((cmd->command_code == CmdCode::JR) || (cmd->command_code == CmdCode::JP)) && (cmd->getArgsCount() == 1)) {
    const auto& arg1 = cmd->getArg(0);
    //FIXME: tmp check, must be removed after debug
    const std::string& arg1str = arg1->toString();
    if (arg1str[0] == '"') {
      throw std::runtime_error("unknown argument");
    }
    if (arg1->arg_type == ArgType::ARG_REGISTER_REF) {
      // posybly need to check register: IX, IY, HL
      //jump to (HL) or (IX) or (IY). address unknown, so we just break disassembling here
      return dasm::core::JumpType::JT_RET;
    }
    jump_addr = cmd->getJmpAddrFromString();
    return dasm::core::JumpType::JT_JUMP;
  }
  if (((cmd->command_code == CmdCode::JR) || (cmd->command_code == CmdCode::JP)) && (cmd->getArgsCount() == 2)) {
    jump_addr = cmd->getJmpAddrFromString();
    return dasm::core::JumpType::JT_COND_JUMP;
  }
  if (cmd->command_code == CmdCode::DJNZ) {
    jump_addr = cmd->getJmpAddrFromString();
    return dasm::core::JumpType::JT_COND_JUMP;
  }
  if ((cmd->command_code == CmdCode::RET) && (cmd->getArgsCount() == 1)) {
    return dasm::core::JumpType::JT_COND_RET;
  }
  if ((cmd->command_code == CmdCode::RET) && (cmd->getArgsCount() == 0)) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd->command_code == CmdCode::RETI) {
    return dasm::core::JumpType::JT_RET;
  }
  if (cmd->command_code == CmdCode::RETN) {
    return dasm::core::JumpType::JT_RET;
  }
  return dasm::core::JumpType::JT_NONE;
}

size_t DisassemblerCore::postProcessChunk(ChunkPtr chunk, size_t len) {
  for (auto& p : _postprocessors) {
    if (p->checkPrecondition(chunk)) {
      len = p->process(chunk, len);
    }
  }
  return len;
}

bool DisassemblerCore::extractAddrFromRef(const std::string& ref, memory::Addr& add_out) {
  //sub_0008: //ignore
  //(word_5c5d)
  //(word_5c5f),
  //jmp_1bd1+27/c
  //sub_0c3b+2/c
  //jmp_0c86/c
  //jmp_0053
  //ffff

  if (ref.find(':') != ref.npos) {
    return false;
  }
  if (ref.find('(') != ref.npos) {
    //data ref
    std::string refnc = ref;
    refnc.erase(std::remove(refnc.begin(), refnc.end(), '('), refnc.end());
    refnc.erase(std::remove(refnc.begin(), refnc.end(), ')'), refnc.end());
    refnc.erase(std::remove(refnc.begin(), refnc.end(), ','), refnc.end());
    auto splited = utils::split(refnc, '_');
    if (splited.size() != 2) {
      std::cerr << "unable to process reference : " << ref << std::endl;
      return false;
      //throw std::runtime_error("unable to process reference: " + ref);
    }
    auto offs = utils::hex2int(splited[1]);
    add_out.setOffset(offs);
    return true;
  }
  if (ref.find('+') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '/');
    auto spl2 = utils::split(spl1[0], '_');
    auto spl3 = utils::split(spl2[1], '+');
    auto addr = utils::hex2int(spl3[0]);
    auto offs = std::atoi(spl3[1].c_str());
    add_out.setOffset(addr + offs);
    return true;
  }
  if (ref.find('/') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '/');
    auto spl2 = utils::split(spl1[0], '_');
    auto offs = utils::hex2int(spl2[1]);
    add_out.setOffset(offs);
    return true;
  }
  if (ref.find('_') != ref.npos) {
    //ref with offset
    std::string refnc = ref;
    auto spl1 = utils::split(refnc, '_');
    auto offs = utils::hex2int(spl1[1]);
    add_out.setOffset(offs);
    return true;
  }
  if (ref.size() == 4) {
    //value
    try {
      auto offs = utils::hex2int(ref);
      add_out.setOffset(offs);
      return true;
    } catch (...) {
      std::cerr << "unable to process reference : " << ref << std::endl;
      return false;
    }
  }
  return false;
}
}
}
