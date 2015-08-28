//
// C++ Interface: CChunk
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCHUNK_H
#define CCHUNK_H
#include "CReference.h"
#include "CCommand.h"

class CChunk {
public:
  using ReferencesList = std::vector<CReference>;
  using CommandsList = std::vector<CCommand>;
  enum class Type {
    UNKNOWN=-1,
    UNPARSED=0,
    CODE=1,
    DATA_BYTE=2,
    DATA_WORD=3,
    DATA_ARRAY=4
  };

  ~CChunk() {};

  CChunk(const CChunk &ch) {makeCopy(ch);}
  CChunk &operator=(const CChunk &ch) { makeCopy(ch); return *this; }

  CChunk* splitAt(CAddr addr);
  void addCrossRef(CAddr addr, CReference::Type type);

  void appendCommand(CCommand cmd);
  CCommand getCommand(int idx) const;
  CCommand lastCommand() const { return m_Commands.back(); }

  int commandsCount() const {return m_Commands.size();}
  CommandsList &commands() {return m_Commands;}

  CAddr addr() const {return m_StartingAddr;}
  Type type() const {return m_Type;}

  std::string label() const {return m_Label;}
  std::string setLabel(std::string label=std::string(), CReference::Type=CReference::Type::JUMP);
  void changeLabel(std::string label) {m_Label=label;}

  ReferencesList& references() {return _references;}
  std::string comment() const {return _comment;}

  void setCursorEndPosition(int pos) {m_EndCursorPosition=pos;}
  void setCursorStartPosition(int pos) {m_StartCursorPosition=pos;}
  int cursorEndPosition() const {return m_EndCursorPosition;}
  int cursorStartPosition() const {return m_StartCursorPosition;}

  bool isEmpty() const { return !((type()!=CChunk::Type::UNPARSED) && (type()!=CChunk::Type::UNKNOWN)); }

private:
  friend class CChunkList;

  CChunk() : m_Length(0) {};
  CChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN) : m_Type(type), m_StartingAddr(addr) {};

  void makeCopy(const CChunk &ch) {
    m_StartingAddr=ch.m_StartingAddr;
    m_Length=ch.m_Length;
    _comment=ch._comment;
    m_Commands=ch.m_Commands;
    _references=ch._references;
    m_Label=ch.m_Label;
    m_Type=ch.m_Type;
    m_StartCursorPosition=ch.m_StartCursorPosition;
    m_EndCursorPosition=ch.m_EndCursorPosition;
  }

  ///@brief Комментарий для блока
  std::string _comment;
  ///@brief Ссылки на блок
  ReferencesList _references;
  ///@brief Метка блока
  std::string m_Label;
  ///@brief Команды блока
  CommandsList m_Commands;
  ///@brief Тип блока
  Type m_Type;
  ///@brief Адрес начала блока
  CAddr m_StartingAddr;
  ///@brief Позиция курсора в редакторе
  ///@bug Должны быть в другом месте
  int m_StartCursorPosition;
  int m_EndCursorPosition;
  ///@brief Длина блока
  unsigned m_Length;
};

#endif
