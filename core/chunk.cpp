//
// C++ Implementation: CChunk
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "chunk.h"
#include "disassembler_core.h"
#include "debug_printers.h"

namespace dasm {
namespace core {

void Chunk::addCrossRef(const memory::Addr& addr, memory::Reference::Type type) {
  memory::Reference ref(addr, type);
  _references.push_back(ref);
}

void Chunk::appendCommand(const Command& cmd) {
  _commands.push_back(cmd);
  _length += cmd.len;
  _last_addr = _starting_addr + _length;
}

Command& Chunk::getCommand(int idx) {
  if (_commands.size() == 0) {
    std::cerr << "No commands here" << std::endl;
    throw int(666);
  }
  return _commands[idx];
}

std::shared_ptr<Label> Chunk::setLabel(std::shared_ptr<Label> label, memory::Reference::Type ref_type) {
  if (label == nullptr) {
    //generate from name
    std::string t1{ _starting_addr.offsetString() };
    switch (ref_type) {
      case memory::Reference::Type::JUMP:
        _label = std::make_shared<Label>(_starting_addr, std::string("jmp_") + t1);
        break;
      case memory::Reference::Type::CALL:
        _label = std::make_shared<Label>(_starting_addr, std::string("sub_") + t1);
        break;
      case memory::Reference::Type::READ_BYTE:
      case memory::Reference::Type::WRITE_BYTE:
        _label = std::make_shared<Label>(_starting_addr, std::string("byte_") + t1);
        break;
      case memory::Reference::Type::READ_WORD:
      case memory::Reference::Type::WRITE_WORD:
        _label = std::make_shared<Label>(_starting_addr, std::string("word_") + t1);
        break;
    }
  } else {
    _label = label;
  }
  return _label;
}

std::shared_ptr<Label> Chunk::setLabel(const std::string& label) {
  _label = std::make_shared<Label>(_starting_addr, label);
  return _label;
}

std::shared_ptr<Chunk> Chunk::splitAt(const memory::Addr& addr) {
  std::cout << "splitAt: commans.count=" << _commands.size() << std::endl;
  if (_commands.size() < 2) {
    //split impossible, too short chunk
    return nullptr;
  }
  CommandsList::iterator it;
  memory::Addr cur_addr = _starting_addr;
  size_t len = 0;
  for (it = _commands.begin(); it != _commands.end(); ++it) {
    std::cout << "caddr" << cur_addr.toString() << std::endl;
    if (cur_addr == addr) {
      //start splitting
      break;
    } else if (cur_addr > addr) {
      //split impossible, allready busy
      return nullptr;
    }
    cur_addr += (*it).len;
    len += (*it).len;
  }
  _length = len;
  _last_addr = _starting_addr + _length;
  std::cout << "moving commands" << std::endl;
  std::shared_ptr<Chunk> new_chunk = DisassemblerCore::inst().createChunk(addr, _type);
  if (new_chunk == nullptr) {
    std::cerr << "ERROR: Can't create chunk" << std::endl;
    return nullptr;
  }
  int cnt = 0;
  len = 0;
  for (; it != _commands.end();) {
    std::cout << "count" << cnt << std::endl;
    cnt++;
    new_chunk->appendCommand(*it);
    it = _commands.erase(it);
    //if (it==m_Commands.end()) break;
  }
  std::cout << "splitAt: commans.count=" << _commands.size() << "new count" << new_chunk->_commands.size() << std::endl;
  return new_chunk;
}

}
}
