//
// C++ Interface: CLabels
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABELS_H
#define CLABELS_H

#include <map>
#include <memory>

#include "label.h"
#include "chunk.h"

class Labels : public std::map<Addr, std::shared_ptr<Label>> {
public:
  ///@brief Вовращает сроку адреса относительно метки.
  std::string offsetInLabel(const Addr &addr) const;

  /**
   * @brief Изменяет метку. Создаёт если не найдена соотв. метка
   * @param chunk Указатель на блок, для которого меняется метка
   * @param new_label Новое название метки
   */
  void changeLabel(const Addr &addr, const std::string &new_label);
};

#endif
