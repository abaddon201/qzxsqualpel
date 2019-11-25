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

#include "core/byte.h"
#include "core/cmd_code.h"
#include "memory/addr.h"

#include <vector>
#include <memory>

#include "core/arg.h"
#include "core/label.h"

namespace dasm {
namespace core {

///@brief Структура описывающая одну команду ассемблера
struct Command {
  ///@brief символьное представление команды
  std::string command;
  ///@brief числовое представление команды (не обязательно опкод)
  CmdCode command_code;
  ///@brief комментарий к команде
  std::string comment;
  ///@brief комментарий к команде
  std::string auto_comment;
  ///@brief Адрес начала команды
  /// @todo Команда может быть разорвана по сегментам
  memory::Addr addr;
  ///@brief Длина команды в байтах
  size_t len;

  Command() : command_code{ CmdCode::NONE }, len{ 0 } {}

  void clone(const Command& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    command = c.command;
    args = c.args;
    comment = c.comment;
    auto_comment = c.auto_comment;
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
    command = c.command;
    args = c.args;
    comment = c.comment;
    auto_comment = c.auto_comment;
    return *this;
  }

  void parse(std::string& src);

  ///@brief Возвращает аргументы команды в виде строки
  std::string getArgsString() const;

  ///@brief Возвращает опкоды команды в виде строки
  /// @todo стоит добавить ограничение на кол-во опкодов
  std::string getOpcodesString() const;

  Byte opcodes(unsigned long long offs) const;

  ///@brief Возвращает адрес перехода команды (первый либо второй аргумент)
  memory::Addr getJmpAddr() const;

  memory::Addr getJmpAddrFromString() const;

  ///@brief Устанавливает метку перехода команды (первый либо второй аргумент)
  void setJmpAddr(const std::shared_ptr<Label> label);

  bool isLDICmd();

  ArgPtr getArg(int idx) { return args[idx]; }

  void setArg(int idx, ArgPtr arg) { if (args.size() < idx + 1) { args.resize(idx + 1); } args[idx] = arg; }

  size_t getArgsCount() { return args.size(); }

private:
  void updateArgs();

  bool isSingleByteArgCmd();

  ArgPtr parseArg(const std::string& arg);

  std::vector<ArgPtr> args;
};

}
}
#endif
