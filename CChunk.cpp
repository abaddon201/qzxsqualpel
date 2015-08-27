//
// C++ Implementation: CChunk
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QDebug>

#include "CChunk.h"
#include "CDisassembler.h"

void CChunk::addCrossRef(CAddr addr, CReference::ReferenceType type) {
  CReference ref(addr, type);
  references.append(ref);
}

void CChunk::appendCommand(CCommand cmd) {
  m_Commands.append(cmd);
  m_Length+=cmd.opcodes.size();
}

CCommand CChunk::getCommand(int idx) {
  if (m_Commands.count()==0) {
    qDebug()<<"No commands here";
    throw int(666);
  }
  return m_Commands[idx];
}

CCommand CChunk::lastCommand() {
  return m_Commands.last();
}

QString CChunk::setLabel(QString label, CReference::ReferenceType ref_type) {
  if (label.isEmpty()) {
    //generate from name
    QString t1=m_StartingAddr.offsetString();
    switch (ref_type) {
    case CReference::JUMP:
      m_Label=QString("jmp_")+t1;
      break;
    case CReference::CALL:
      m_Label=QString("sub_")+t1;
      break;
    case CReference::READ_BYTE:
    case CReference::WRITE_BYTE:
      m_Label=QString("byte_")+t1;
      break;
    case CReference::READ_WORD:
    case CReference::WRITE_WORD:
      m_Label=QString("word_")+t1;
      break;
    }
  } else {
    m_Label=label;
  }
  return m_Label;
}

CChunk* CChunk::splitAt(CAddr addr) {
  qDebug()<<"splitAt: commans.count="<<m_Commands.count();
  if (m_Commands.count()<2) {
    //split impossible, too short chunk
    return 0;
  }
  QList<CCommand>::iterator it;
  CAddr cur_addr=m_StartingAddr;
  for (it=m_Commands.begin(); it!=m_Commands.end(); ++it) {
    cur_addr+=(*it).opcodes.count();
    qDebug()<<"caddr"<<cur_addr.toString();
    if (cur_addr==addr) {
      //start splitting
      break;
    } else if (cur_addr>addr) {
      //split impossible, allready busy
      return 0;
    }
  }
  qDebug()<<"moving commands";
  CChunk* new_chunk=CDisassembler::inst()->createChunk(addr, m_Type);
  if (new_chunk==0) {
    qDebug()<<"can't create chunk";
    return 0;
  }
  int cnt=0;
  for (; it!=m_Commands.end();) {
    qDebug()<<"count"<<cnt;
    cnt++;
    //new_chunk->appendCommand(*it);
    new_chunk->m_Commands.append(*it);
    it=m_Commands.erase(it);
    //if (it==m_Commands.end()) break;
  }
  qDebug()<<"splitAt: commans.count="<<m_Commands.count()<<"new count"<<new_chunk->m_Commands.count();
  return new_chunk;
}

QDebug operator<<(QDebug out, CChunk::Type t) {
  switch (t) {
  case CChunk::Type::UNKNOWN:
    out<<"UNKNOWN";
    break;
  case CChunk::Type::UNPARSED:
    out<<"UNPARSED";
    break;
  case CChunk::Type::CODE:
    out<<"CODE";
    break;
  case CChunk::Type::DATA_BYTE:
    out<<"DATA_BYTE";
    break;
  case CChunk::Type::DATA_WORD:
    out<<"DATA_WORD";
    break;
  case CChunk::Type::DATA_ARRAY:
    out<<"DATA_ARRAY";
    break;
  }
  return out;
}
