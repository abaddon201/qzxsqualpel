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

#include <memory>

#include "CReference.h"
#include "CCommand.h"
#include "CLabel.h"

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

  ~CChunk() {}

  CChunk(const CChunk &ch) {makeCopy(ch);}
  CChunk &operator=(const CChunk &ch) { makeCopy(ch); return *this; }

  std::shared_ptr<CChunk> splitAt(CAddr addr);
  void addCrossRef(CAddr addr, CReference::Type type);

  void appendCommand(CCommand cmd);
  CCommand& getCommand(int idx) ;
  CCommand& lastCommand() { return m_Commands.back(); }

  inline int commandsCount() const {return m_Commands.size();}
  inline CommandsList &commands() {return m_Commands;}

  inline const CAddr& addr() const {return m_StartingAddr;}
  inline bool containsAddr(const CAddr& addr) const {return (addr>=m_StartingAddr) && (addr<_last_addr);}

  inline unsigned long long length() const {return m_Length;}

  inline Type type() const {return m_Type;}

  std::shared_ptr<CLabel> label() const {return m_Label;}
  std::shared_ptr<CLabel> setLabel(std::shared_ptr<CLabel> label = nullptr, CReference::Type=CReference::Type::JUMP);
  std::shared_ptr<CLabel> setLabel(std::string label, CReference::Type=CReference::Type::JUMP);
//  void changeLabel(std::string label) {m_Label=label;}

  ReferencesList& references() {return _references;}
  std::string comment() const {return _comment;}

  bool isEmpty() const { return !((type()!=CChunk::Type::UNPARSED) && (type()!=CChunk::Type::UNKNOWN)); }

  CChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN) : m_Type(type), m_StartingAddr(addr), m_Length{0} {}

private:
//  friend class CChunkList;

  CChunk() : m_Length(0) {}

  virtual void makeCopy(const CChunk &ch) {
    m_StartingAddr=ch.m_StartingAddr;
    _last_addr=ch._last_addr;
    m_Length=ch.m_Length;
    _comment=ch._comment;
    m_Commands=ch.m_Commands;
    _references=ch._references;
    m_Label=ch.m_Label;
    m_Type=ch.m_Type;
  }

  ///@brief Комментарий для блока
  std::string _comment;
  ///@brief Ссылки на блок
  ReferencesList _references;
  ///@brief Метка блока
  std::shared_ptr<CLabel> m_Label;
  ///@brief Команды блока
  CommandsList m_Commands;
  ///@brief Тип блока
  Type m_Type;
  ///@brief Адрес начала блока
  CAddr m_StartingAddr;
  CAddr _last_addr;
  ///@brief Длина блока
  unsigned long long m_Length;
};

#endif
