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

#include "labels.h"
#include "chunk.h"
#include "chunk_list.h"
#include "i_disassembler_core.h"

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
std::string Labels::offsetInLabel(const Addr &addr) const {
  if (size() == 0) {
    std::cout << "no labels" << std::endl;
    return addr.toString();
  }
  std::shared_ptr<Chunk> chunk = IDisassemblerCore::inst()->chunks().getChunkContains(addr);
  if (chunk == nullptr) {
    std::cout << "no label for addr" << std::endl;
    return addr.toString();
  }
  Addr ch_addr = chunk->addr();
  std::cout << "addr:" << ch_addr.toString() << std::endl;
  std::shared_ptr<Label> lbl = chunk->label();
  if (lbl == nullptr) {
    std::cout << "no label for chunk" << std::endl;
    return addr.toString();
  }
  Addr delta = addr - ch_addr;
  if (delta == 0) {
    return lbl->name;
  }
  return lbl->name + "+" + std::to_string(delta.offset());
}

void Labels::changeLabel(const Addr &addr, const std::string &new_label) {
  this->at(addr)->name = new_label;
}
