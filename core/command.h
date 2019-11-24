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
#include "core/jump_type.h"
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
  JumpCmd command_code;
  ///@brief символьное представление первого аргумента
  std::shared_ptr<Arg> arg1;
  ///@brief символьное представление второго аргумента
  std::shared_ptr<Arg> arg2;
  ///@brief комментарий к команде
  std::string comment;
  ///@brief комментарий к команде
  std::string auto_comment;
  ///@brief Адрес начала команды
  /// @todo Команда может быть разорвана по сегментам
  memory::Addr addr;
  ///@brief Длина команды в байтах
  size_t len;

  Command() : command_code{ JumpCmd::CMD_NONE }, len{ 0 } {}

  Command(const Command& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    command = c.command;
    arg1 = c.arg1;
    arg2 = c.arg2;
    comment = c.comment;
    auto_comment = c.auto_comment;
  }

  Command& operator=(const Command& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    command = c.command;
    arg1 = c.arg1;
    arg2 = c.arg2;
    comment = c.comment;
    auto_comment = c.auto_comment;
    return *this;
  }

  Command& operator=(const Command&& c) {
    addr = c.addr;
    len = c.len;
    command_code = c.command_code;
    command = c.command;
    arg1 = c.arg1;
    arg2 = c.arg2;
    comment = c.comment;
    auto_comment = c.auto_comment;
    return *this;
  }

  JumpCmd Command::command2code(const std::string& cmd) const;

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

};

}
}
#endif
