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
    std::cout<<"no labels"<<std::endl;
    return addr.toString();
  }
  std::shared_ptr<CChunk> chunk=IDisassemblerCore::inst()->chunks().getChunkContains(addr);
  if (chunk==nullptr) {
    std::cout<<"no label for addr"<<std::endl;
    return addr.toString();
  }
  CAddr ch_addr=chunk->addr();
  std::cout<<"addr:"<<ch_addr.toString()<<std::endl;
  std::shared_ptr<CLabel> lbl=chunk->label();
  if (lbl == nullptr) {
    std::cout<<"no label for chunk"<<std::endl;
    return addr.toString();
  }
  CAddr delta=addr-ch_addr;
  if (delta == 0) {
    return lbl->name;
  }
  return lbl->name + "+" + std::to_string(delta.offset());
}

void CLabels::changeLabel(const CAddr &addr, const std::string new_label) {
  this->at(addr)->name = new_label;
}
