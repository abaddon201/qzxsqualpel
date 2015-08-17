//
// C++ Interface: CByte
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CBYTE_H
#define CBYTE_H

#include <QString>

class CByte {
public:
    CByte(unsigned char b=0) : m_Byte(b) {}
    unsigned char toUChar() const {
        return m_Byte;
    }
    QString toString() const;
private:
    unsigned char m_Byte;
};

#endif
