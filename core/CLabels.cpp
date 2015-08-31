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

/**
 * @brief Вовращает сроку адреса относительно метки.
 * @param addr Искомый адрес
 * @return Отформатированная строка для отображения метки
 *
 * 1. Если существует блок, покрывающий запрошенный адрес, берётся метка этого блока, в противном случае, возвращается
 * строка вида <address>
 * 2. Если метка существует, то вычисляется смещение запрошенного адреса, относительно метки
 * и формируется строка вида <label>+<offset>, если смещение не нулевое, и <label>, в противном случае
 */
std::string CLabels::offsetInLabel(const CAddr &addr) const {
  if (size()==0) {
    Debug()<<"no labels";
    return addr.toString();
  }
  std::shared_ptr<CChunk> chunk=IDisassemblerCore::inst()->chunks().getChunkContains(addr);
  if (chunk==nullptr) {
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
  if (delta == 0) {
    return lbl;
  }
  return lbl+"+"+std::to_string(delta.offset());
}

void CLabels::changeLabel(std::shared_ptr<CChunk> chunk, const std::string new_label) {
  chunk->changeLabel(new_label);
  for (auto lbl: *this) {
    if (lbl.addr == chunk->addr()) {
      //change and return;
      lbl.name = new_label;
      return;
    }
  }
  CLabel lbl(chunk->addr(), new_label);
  push_back(lbl);
}
