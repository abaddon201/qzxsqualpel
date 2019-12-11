//
// C++ Implementation: CLabels
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "utils/utils.h"
#include "labels.h"
#include "command.h"
#include "disassembler_core.h"

#include "utils/plog/Log.h"

namespace dasm {
namespace core {

LabelPtr Labels::getByName(const std::string& name) {
  auto res= std::find_if(begin(), end(), [&name](auto v) ->bool{
    return v.second->name == name;
  });
  if (res != end()) {
    return res->second;
  }
  return nullptr;
}

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
std::string Labels::offsetInLabel(uint16_t addr) const {
  if (size() == 0) {
    PLOGD << "no labels" << std::endl;
    return utils::toHex(addr);
  }
  auto& cmd = DisassemblerCore::inst().commands().get(addr);
  if (cmd == nullptr) {
    PLOGD << "no label for addr" << std::endl;
    return utils::toHex(addr);
  }
  auto ch_addr = cmd->addr;
  PLOGD << "addr:" << utils::toHex(ch_addr) << std::endl;
  auto lbl = cmd->label();
  if (lbl == nullptr) {
    PLOGD << "no label for chunk" << std::endl;
    return utils::toHex(addr);
  }
  uint16_t delta = addr - ch_addr;
  if (delta == 0) {
    return lbl->name;
  }
  return lbl->name + "+" + std::to_string(delta);
}

void Labels::changeLabel(uint16_t addr, const std::string& new_label) {
  this->at(addr)->name = new_label;
}

}
}
