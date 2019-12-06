//
// C++ Interface: CCommand
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCOMMAND_H
#define CCOMMAND_H

#include "core/cmd_code.h"
#include "memory/reference.h"

#include <vector>
#include <memory>

#include "core/arg.h"
#include "core/label.h"

namespace dasm {
namespace core {

///@brief Структура описывающая одну команду ассемблера
struct Command {
  using ReferencesList = std::vector<memory::ReferencePtr>;

  ///@brief символьное представление команды
  //std::string command;
  ///@brief числовое представление команды (не обязательно опкод)
  CmdCode command_code;
  ///@brief комментарий к команде
  std::string comment;
  ///@brief комментарий к команде
  std::string auto_comment;
  ///@brief Адрес начала команды
  /// @todo Команда может быть разорвана по сегментам
  uint16_t addr;
  ///@brief Длина команды в байтах
  size_t len;

  Command() : command_code{ CmdCode::NONE }, len{ 0 } {}

  void clone(const Command& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    //command = c.command;
    _args = c._args;
    comment = c.comment;
    auto_comment = c.auto_comment;
    _label = c._label;
    _references = c._references;
    _blockComment = c._blockComment;
  }
  Command(const Command& c) {
    clone(c);
  }

  Command& operator=(const Command& c) {
    clone(c);
    return *this;
  }

  Command& operator=(const Command&& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    //command = c.command;
    _args = c._args;
    comment = c.comment;
    auto_comment = c.auto_comment;
    _label = c._label;
    _references = c._references;
    _blockComment = c._blockComment;
    return *this;
  }

  void parse(std::string& src);

  ///@brief Возвращает аргументы команды в виде строки
  std::string getArgsString() const;

  ///@brief Возвращает опкоды команды в виде строки
  /// @todo стоит добавить ограничение на кол-во опкодов
  std::string getOpcodesString(size_t opcodes_count = -1) const;

  uint8_t opcodes(unsigned long long offs) const;

  ///@brief Возвращает адрес перехода команды (первый либо второй аргумент)
  uint16_t getJmpAddr() const;
  JumpType jumpType(uint16_t& jump_addr);

  bool containsAddr(const uint16_t a) const {
    return a >= addr && a < addr + len;
  }

  uint16_t getJmpAddrFromString() const;

  ///@brief Устанавливает метку перехода команды (первый либо второй аргумент)
  void setJmpAddr(const LabelPtr label);

  bool isLDICmd();

  ArgPtr getArg(int idx) { return _args[idx]; }
  void setArg(int idx, ArgPtr arg) { if (_args.size() < idx + 1) { _args.resize(idx + 1); } _args[idx] = arg; }
  size_t getArgsCount() const { return _args.size(); }
  const std::vector<ArgPtr>& args() const { return _args; }
  std::vector<ArgPtr>& args() { return _args; }
  void setArgs(std::vector<ArgPtr>& args) { _args = args; }

  LabelPtr label() const { return _label; }
  LabelPtr setLabel(LabelPtr label = nullptr, memory::Reference::Type = memory::Reference::Type::JUMP);
  //void setLabel(const std::string& label);

  void addCrossRef(uint16_t addr, memory::Reference::Type type);
  ReferencesList& references() { return _references; }

  const std::string& blockComment() const { return _blockComment; }
  void setBlockComment(const std::string& comm) { _blockComment = comm; }

  void setComment(const std::string& comm) { comment = comm; }
  void setAutoComment(const std::string& comm) { auto_comment = comm; }
private:
  void updateArgs();

  bool isSingleByteArgCmd();

  ArgPtr parseArg(const std::string& arg);

  std::vector<ArgPtr> _args;
  LabelPtr _label;
  ReferencesList _references;
  std::string _blockComment;

};

using CommandPtr = std::shared_ptr<Command>;
}
}
#endif
