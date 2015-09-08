//
// C++ Interface: CCommand
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCOMMAND_H
#define CCOMMAND_H

#include "byte.h"
#include "addr.h"

#include <vector>
#include <memory>

#include "arg.h"
#include "label.h"

///@brief Структура описывающая одну команду ассемблера
struct Command {
  //using OpcodesList = std::vector<CByte>;
  enum class ArgType {
    ARG_UNKNOWN=0,
    ARG_REGISTER=1,
    ARG_DATAOFFSET=2,
    ARG_JUMPOFFSET=3,
    ARG_VALUE=4,
    ARG_VALOFFSET=5,
    ARG_FLAG=6
  };

  //OpcodesList opcodes;
  ///@brief символьное представление команды
  std::string command;
  ///@brief числовое представление команды (не обязательно опкод)
  int command_code;
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
  Addr addr;
  ///@brief Длина команды в байтах
  int len;

  Command() : command_code{0}, len{0} {}
  Command(const Command &c) {
    addr=c.addr;
    len=c.len;
    command_code=c.command_code;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
    auto_comment=c.auto_comment;
  }
  Command &operator=(const Command &c) {
    addr=c.addr;
    len=c.len;
    command_code=c.command_code;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
    auto_comment=c.auto_comment;
    return *this;
  }

  Command &operator=(const Command &&c) {
    addr=c.addr;
    len=c.len;
    command_code=c.command_code;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
    auto_comment=c.auto_comment;
    return *this;
  }

  ///@brief Возвращает аргументы команды в виде строки
  std::string getArgsString() const;
  ///@brief Возвращает опкоды команды в виде строки
  /// @todo стоит добавить ограничение на кол-во опкодов
  std::string getOpcodesString() const;
  Byte opcodes(unsigned long long offs) const;
  ///@brief Возвращает адрес перехода команды (первый либо второй аргумент)
  Addr getJmpAddr() const;
  Addr getJmpAddrFromString() const;
  ///@brief Устанавливает метку перехода команды (первый либо второй аргумент)
  void setJmpAddr(const std::shared_ptr<Label> label);
};

#endif
