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
  m_Length+=cmd.len;
  _last_addr = m_StartingAddr + m_Length;
}

CCommand &CChunk::getCommand(int idx) {
  if (m_Commands.size()==0) {
    std::cerr<<"No commands here"<<std::endl;
    throw int(666);
  }
  return m_Commands[idx];
}

std::shared_ptr<CLabel> CChunk::setLabel(std::shared_ptr<CLabel> label, CReference::Type ref_type) {
  if (label==nullptr) {
    //generate from name
    std::string t1{m_StartingAddr.offsetString()};
    switch (ref_type) {
    case CReference::Type::JUMP:
      m_Label=std::make_shared<CLabel>(m_StartingAddr, std::string("jmp_")+t1);
      break;
    case CReference::Type::CALL:
      m_Label=std::make_shared<CLabel>(m_StartingAddr, std::string("sub_")+t1);
      break;
    case CReference::Type::READ_BYTE:
    case CReference::Type::WRITE_BYTE:
      m_Label=std::make_shared<CLabel>(m_StartingAddr, std::string("byte_")+t1);
      break;
    case CReference::Type::READ_WORD:
    case CReference::Type::WRITE_WORD:
      m_Label=std::make_shared<CLabel>(m_StartingAddr, std::string("word_")+t1);
      break;
    }
  } else {
    m_Label = label;
  }
  return m_Label;
}

std::shared_ptr<CLabel> CChunk::setLabel(std::string label, CReference::Type ref_type) {
  m_Label = std::make_shared<CLabel>(m_StartingAddr, label);
  return m_Label;
}

std::shared_ptr<CChunk> CChunk::splitAt(CAddr addr) {
  std::cout<<"splitAt: commans.count="<<m_Commands.size()<<std::endl;
  if (m_Commands.size()<2) {
    //split impossible, too short chunk
    return 0;
  }
  CommandsList::iterator it;
  CAddr cur_addr=m_StartingAddr;
  int len=0;
  for (it=m_Commands.begin(); it!=m_Commands.end(); ++it) {
    std::cout<<"caddr"<<cur_addr.toString()<<std::endl;
    if (cur_addr==addr) {
      //start splitting
      break;
    } else if (cur_addr>addr) {
      //split impossible, allready busy
      return nullptr;
    }
    cur_addr+=(*it).len;
    len+=(*it).len;
  }
  m_Length = len;
  _last_addr = m_StartingAddr + m_Length;
  std::cout<<"moving commands"<<std::endl;
  std::shared_ptr<CChunk> new_chunk=IDisassemblerCore::inst()->createChunk(addr, m_Type);
  if (new_chunk==nullptr) {
    std::cerr<<"ERROR: Can't create chunk"<<std::endl;
    return nullptr;
  }
  int cnt=0;
  len = 0;
  for (; it!=m_Commands.end();) {
    std::cout<<"count"<<cnt<<std::endl;
    cnt++;
    new_chunk->appendCommand(*it);
    it=m_Commands.erase(it);
    //if (it==m_Commands.end()) break;
  }
  std::cout<<"splitAt: commans.count="<<m_Commands.size()<<"new count"<<new_chunk->m_Commands.size()<<std::endl;
  return new_chunk;
}
