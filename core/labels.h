//
// C++ Interface: CLabels
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABELS_H
#define CLABELS_H

#include <map>
#include <memory>

#include "label.h"

namespace dasm {
namespace core {

class Labels : public std::map<uint16_t, LabelPtr> {
public:
  ///@brief Вовращает сроку адреса относительно метки.
  std::string offsetInLabel(uint16_t addr) const;

  /**
   * @brief Изменяет метку. Создаёт если не найдена соотв. метка
   * @param chunk Указатель на блок, для которого меняется метка
   * @param new_label Новое название метки
   */
  void changeLabel(uint16_t addr, const std::string& new_label);
};

}
}

#endif
