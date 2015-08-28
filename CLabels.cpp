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

#include <QDebug>

#include "CLabels.h"
#include "CChunk.h"
#include "CDisassembler.h"

QString CLabels::offsetInLabel(CAddr &addr) const {
  if (count()==0) {
    qDebug()<<"no labels";
    return addr.toString();
  }
  CChunk* chunk=CDisassembler::core_inst()->chunks().getChunkContains(addr);
  if (chunk==0) {
    qDebug()<<"no label for addr";
    return addr.toString();
  }
  CAddr ch_addr=chunk->addr();
  qDebug()<<"addr:"<<ch_addr.toString();
  QString lbl=chunk->label();
  if (lbl.isEmpty()) {
    qDebug()<<"no label for chunk";
    return addr.toString();
  }
  CAddr delta=addr-ch_addr;
  //unsigned delta=;
  return lbl+"+"+QString::number(delta.offset(), 10);
}


void CLabels::changeLabel(CChunk* chunk, QString new_label) {
  chunk->changeLabel(new_label);
  QList<CLabel>::iterator it;
  for (it=begin(); it!=end(); ++it) {
    CLabel &lbl=*it;
    if (lbl.addr==chunk->addr()) {
      //change and return;
      lbl.name=new_label;
      return;
    }
  }
  CLabel lbl(chunk->addr(), new_label);
  append(lbl);
}
