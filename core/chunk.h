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

#include "reference.h"
#include "command.h"
#include "label.h"

namespace dasm {
namespace core {

class Chunk {
public:
  using ReferencesList = std::vector<Reference>;
  using CommandsList = std::vector<Command>;
  using size_type = size_t;

  enum class Type {
    UNKNOWN = -1,
    UNPARSED = 0,
    CODE = 1,
    DATA_BYTE = 2,
    DATA_WORD = 3,
    DATA_ARRAY = 4
  };

  ~Chunk() = default;

  Chunk(const Chunk& ch) { makeCopy(ch); }

  explicit Chunk(const Addr& addr, Chunk::Type type = Chunk::Type::UNKNOWN) : _type(type), _starting_addr(addr), _length{ 0 } {}

  Chunk& operator=(const Chunk& ch) {
    makeCopy(ch);
    return *this;
  }

  std::shared_ptr<Chunk> splitAt(const Addr& addr);

  void addCrossRef(const Addr& addr, Reference::Type type);

  void appendCommand(const Command& cmd);

  Command& getCommand(int idx);

  Command& lastCommand() { return _commands.back(); }

  inline int commandsCount() const { return _commands.size(); }

  inline CommandsList& commands() { return _commands; }

  inline const Addr& addr() const { return _starting_addr; }

  inline bool containsAddr(const Addr& addr) const { return (addr >= _starting_addr) && (addr < _last_addr); }

  inline size_type length() const { return _length; }

  inline Type type() const { return _type; }

  std::shared_ptr<Label> label() const { return _label; }

  std::shared_ptr<Label> setLabel(std::shared_ptr<Label> label = nullptr, Reference::Type = Reference::Type::JUMP);

  std::shared_ptr<Label> setLabel(const std::string& label, Reference::Type = Reference::Type::JUMP);

  /*/const /
  void cha
  &
  ngeLabel(std::string
  label) { m_Label = label; }
*/
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
  Addr _starting_addr;
  Addr _last_addr;
  ///@brief Длина блока
  size_type _length{};
};

}
}

#endif
