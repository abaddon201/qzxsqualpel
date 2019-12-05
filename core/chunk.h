//
// C++ Interface: CChunk
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCHUNK_H
#define CCHUNK_H

#include <memory>

#include "memory/reference.h"
#include "command.h"
#include "label.h"

namespace dasm {
namespace core {

class Chunk {
public:
  using ReferencesList = std::vector<memory::ReferencePtr>;
  using CommandsList = std::vector<CommandPtr>;
  using size_type = size_t;

  enum class Type {
    UNKNOWN = -1,
    UNPARSED = 0,
    CODE = 1,
    DATA_BYTE = 2,
    DATA_WORD = 3,
    DATA_BYTE_ARRAY = 4,
    DATA_WORD_ARRAY = 5
  };

  ~Chunk() = default;

  Chunk() {}

  Chunk(const Chunk& ch) { makeCopy(ch); }

  explicit Chunk(const memory::Addr& addr, Chunk::Type type = Chunk::Type::UNKNOWN) : _type(type), _starting_addr(addr) {
    if (_type == Type::DATA_BYTE) {
      _length = 1;
    } else if (_type == Type::DATA_WORD) {
      _length = 2;
    } else {
      _length = 0;
    }
  }

  Chunk& operator=(const Chunk& ch) {
    makeCopy(ch);
    return *this;
  }

  bool isSimpleData() { return _type == Type::DATA_BYTE || _type == Type::DATA_WORD; }

  std::shared_ptr<Chunk> splitAt(const memory::Addr& addr);

  void addCrossRef(const memory::Addr& addr, memory::Reference::Type type);

  void appendCommand(const CommandPtr cmd);

  CommandPtr getCommand(int idx);

  CommandPtr getCommand(const memory::Addr& addr);

  CommandPtr lastCommand() { return _commands.back(); }

  inline size_t commandsCount() const { return _commands.size(); }

  inline CommandsList& commands() { return _commands; }

  inline const memory::Addr& addr() const { return _starting_addr; }
  inline void setStartAddr(memory::Addr& addr) { _starting_addr = addr; }

  inline const memory::Addr& lastAddr() const { return _last_addr; }
  inline void setLastAddr(memory::Addr& addr) { _last_addr = addr; }

  inline bool containsAddr(const memory::Addr& addr) const { return (addr >= _starting_addr) && (addr < _last_addr); }

  inline size_type length() const { return _length; }
  inline void setLength(size_type len) { _length = len; }

  inline Type type() const { return _type; }
  inline void setType(Type type) { _type = type; }

  LabelPtr label() const { return _label; }

  LabelPtr setLabel(LabelPtr label = nullptr, memory::Reference::Type = memory::Reference::Type::JUMP);

  void setLabel(const std::string& label);

  ReferencesList& references() { return _references; }

  std::string comment() const { return _comment; }
  void setComment(const std::string& cm) { _comment = cm; }

  bool isEmpty() const { return !((type() != Chunk::Type::UNPARSED) && (type() != Chunk::Type::UNKNOWN)); }

private:

  virtual void makeCopy(const Chunk& ch) {
    _starting_addr = ch._starting_addr;
    _last_addr = ch._last_addr;
    _length = ch._length;
    _comment = ch._comment;
    _commands = ch._commands;
    _references = ch._references;
    _label = ch._label;
    _type = ch._type;
  }

  ///@brief Комментарий для блока
  std::string _comment;
  ///@brief Ссылки на блок
  ReferencesList _references;
  ///@brief Метка блока
  LabelPtr _label;
  ///@brief Команды блока
  CommandsList _commands;
  ///@brief Тип блока
  Type _type;
  ///@brief Адрес начала блока
  memory::Addr _starting_addr;
  memory::Addr _last_addr;
  ///@brief Длина блока
  size_type _length{};
};

using ChunkPtr = std::shared_ptr<Chunk>;

}
}

#endif
