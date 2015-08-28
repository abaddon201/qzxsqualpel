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

#include <list>
#include "CLabel.h"
#include "CChunk.h"

class CLabels : public std::vector<CLabel> {
public:
  std::string offsetInLabel(CAddr &addr) const;
  void changeLabel(CChunk* chunk, std::string new_label);
};

#endif
