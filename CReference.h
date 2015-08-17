//
// C++ Interface: CRefernce
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CREFERENCE_H
#define CREFERENCE_H

#include "CAddr.h"

class CReference {
public:
    typedef enum {
        JUMP=1,
        CALL,
        READ_BYTE,
        WRITE_BYTE,
        READ_WORD,
        WRITE_WORD
    } ReferenceType;
public:
    CReference(CAddr addr, ReferenceType type);
    QString toString();
private:
    CAddr m_From;
    ReferenceType m_Type;
};

#endif
