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

#include "CChunk.h"
#include "IDisassemblerCore.h"
#include "debug_printers.h"

void CChunk::addCrossRef(CAddr addr, CReference::Type type) {
  CReference ref(addr, type);
  _references.push_back(ref);
}

void CChunk::appendCommand(CCommand cmd) {
  m_Commands.push_back(cmd);
  m_Length+=cmd.opcodes.size();
}

CCommand CChunk::getCommand(int idx) const {
  if (m_Commands.size()==0) {
    qDebug()<<"No commands here";
    throw int(666);
  }
  return m_Commands[idx];
}

std::string CChunk::setLabel(std::string label, CReference::Type ref_type) {
  if (label.empty()) {
    //generate from name
    std::string t1{m_StartingAddr.offsetString()};
    switch (ref_type) {
    case CReference::Type::JUMP:
      m_Label=std::string("jmp_")+t1;
      break;
    case CReference::Type::CALL:
      m_Label=std::string("sub_")+t1;
      break;
    case CReference::Type::READ_BYTE:
    case CReference::Type::WRITE_BYTE:
      m_Label=std::string("byte_")+t1;
      break;
    case CReference::Type::READ_WORD:
    case CReference::Type::WRITE_WORD:
      m_Label=std::string("word_")+t1;
      break;
    }
  } else {
    m_Label=label;
  }
  return m_Label;
}

std::shared_ptr<CChunk> CChunk::splitAt(CAddr addr) {
  qDebug()<<"splitAt: commans.count="<<m_Commands.size();
  if (m_Commands.size()<2) {
    //split impossible, too short chunk
    return 0;
  }
  CommandsList::iterator it;
  CAddr cur_addr=m_StartingAddr;
  for (it=m_Commands.begin(); it!=m_Commands.end(); ++it) {
    cur_addr+=(*it).opcodes.size();
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
  std::shared_ptr<CChunk> new_chunk=IDisassemblerCore::inst()->createChunk(addr, m_Type);
  if (new_chunk==0) {
    qDebug()<<"can't create chunk";
    return 0;
  }
  int cnt=0;
  for (; it!=m_Commands.end();) {
    qDebug()<<"count"<<cnt;
    cnt++;
    //new_chunk->appendCommand(*it);
    new_chunk->m_Commands.push_back(*it);
    it=m_Commands.erase(it);
    //if (it==m_Commands.end()) break;
  }
  qDebug()<<"splitAt: commans.count="<<m_Commands.size()<<"new count"<<new_chunk->m_Commands.size();
  return new_chunk;
}
