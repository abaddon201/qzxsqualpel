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

#include <QList>
#include "CLabel.h"
#include "CChunk.h"

class CLabels : public QList<CLabel> {
    public:
        QString offsetInLabel(CAddr& addr) const;
        void changeLabel(CChunk* chunk, QString new_label);
};

#endif
