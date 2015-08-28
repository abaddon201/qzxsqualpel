//
// C++ Implementation: CLabels
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "CLabels.h"
#include "CChunk.h"
#include "CChunkList.h"
#include "IDisassemblerCore.h"

#include "debug_printers.h"

std::string CLabels::offsetInLabel(CAddr &addr) const {
  if (size()==0) {
    Debug()<<"no labels";
    return addr.toString();
  }
  CChunk* chunk=IDisassemblerCore::inst()->chunks().getChunkContains(addr);
  if (chunk==0) {
    Debug()<<"no label for addr";
    return addr.toString();
  }
  CAddr ch_addr=chunk->addr();
  Debug()<<"addr:"<<ch_addr.toString();
  std::string lbl=chunk->label();
  if (lbl.empty()) {
    Debug()<<"no label for chunk";
    return addr.toString();
  }
  CAddr delta=addr-ch_addr;
  //unsigned delta=;
  return lbl+"+"+std::to_string(delta.offset());
}


void CLabels::changeLabel(CChunk* chunk, std::string new_label) {
  chunk->changeLabel(new_label);
  CLabels::iterator it;
  for (it=begin(); it!=end(); ++it) {
    CLabel &lbl=*it;
    if (lbl.addr==chunk->addr()) {
      //change and return;
      lbl.name=new_label;
      return;
    }
  }
  CLabel lbl(chunk->addr(), new_label);
  push_back(lbl);
}
