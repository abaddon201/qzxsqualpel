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

#include "CByte.h"
#include "CAddr.h"

#include <vector>

///@brief Структура описывающая одну команду ассемблера
struct CCommand {
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
  std::string arg1;
  ///@brief символьное представление второго аргумента
  std::string arg2;
  ///@brief комментарий к команде
  std::string comment;
  ///@brief комментарий к команде
  std::string auto_comment;
  ///@brief Адрес начала команды
  /// @todo Команда может быть разорвана по сегментам
  CAddr addr;
  ///@brief Длина команды в байтах
  int len;

  CCommand() : command_code{0}, len{0} {}
  CCommand(const CCommand &c) {
    addr=c.addr;
    len=c.len;
    command_code=c.command_code;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
    auto_comment=c.auto_comment;
  }
  CCommand &operator=(const CCommand &c) {
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
  CByte opcodes(unsigned long long offs) const;
  ///@brief Возвращает адрес перехода команды (первый либо второй аргумент)
  CAddr getJmpAddr() const;
  ///@brief Устанавливает метку перехода команды (первый либо второй аргумент)
  void setJmpAddr(const std::string label);
};

#endif
