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
  using ReferencesList = std::vector<memory::Reference>;
  using CommandsList = std::vector<Command>;
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

  void appendCommand(const Command& cmd);

  Command& getCommand(int idx);

  Command& getCommand(const memory::Addr& addr);

  Command& lastCommand() { return _commands.back(); }

  inline size_t commandsCount() const { return _commands.size(); }

  inline CommandsList& commands() { return _commands; }

  inline const memory::Addr& addr() const { return _starting_addr; }

  inline bool containsAddr(const memory::Addr& addr) const { return (addr >= _starting_addr) && (addr < _last_addr); }

  inline size_type length() const { return _length; }

  inline Type type() const { return _type; }

  std::shared_ptr<Label> label() const { return _label; }

  std::shared_ptr<Label> setLabel(std::shared_ptr<Label> label = nullptr, memory::Reference::Type = memory::Reference::Type::JUMP);

  void setLabel(const std::string& label);

  ReferencesList& references() { return _references; }

  std::string comment() const { return _comment; }

  bool isEmpty() const { return !((type() != Chunk::Type::UNPARSED) && (type() != Chunk::Type::UNKNOWN)); }

private:
  //  friend class CChunkList;

  Chunk() : _length(0) {}

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
  std::shared_ptr<Label> _label;
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
