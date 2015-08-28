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
  using OpcodesList = std::vector<CByte>;
  enum class ArgType {
    ARG_UNKNOWN=0,
    ARG_REGISTER=1,
    ARG_DATAOFFSET=2,
    ARG_JUMPOFFSET=3,
    ARG_VALUE=4,
    ARG_VALOFFSET=5,
    ARG_FLAG=6
  };

  OpcodesList opcodes;
  std::string command;
  std::string arg1;
  std::string arg2;
  std::string comment;
  CAddr addr;

  CCommand() {}
  CCommand(const CCommand &c) {
    addr=c.addr;
    opcodes=c.opcodes;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
  }
  CCommand &operator=(const CCommand &c) {
    addr=c.addr;
    opcodes=c.opcodes;
    command=c.command;
    arg1=c.arg1;
    arg2=c.arg2;
    comment=c.comment;
    return *this;
  }

  ///@brief Возвращает аргументы команды в виде строки
  std::string getArgsString() const;
  ///@brief Возвращает опкоды команды в виде строки
  /// @todo стоит добавить ограничение на кол-во опкодов
  std::string getOpcodesString() const;
  ///@brief Возвращает адрес перехода команды (первый либо второй аргумент)
  CAddr getJmpAddr() const;
};

#endif