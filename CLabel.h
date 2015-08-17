//
// C++ Interface: CLabel
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABEL_H
#define CLABEL_H

#include "CAddr.h"

class CLabel {
public:
    CLabel() {}
    CLabel(CAddr addr, QString name) : m_Name(name), m_Addr(addr) {}
    CLabel(const CLabel& s) {
        m_Addr=s.m_Addr;
        m_Name=s.m_Name;
    }
    CLabel& operator = (const CLabel& s) {
        m_Addr=s.m_Addr;
        m_Name=s.m_Name;
        return *this;
    }
    ~CLabel() {}
    QString name() const {
        return m_Name;
    }
    CAddr addr() const {
        return m_Addr;
    }
    void setAddr(const CAddr& addr) {m_Addr=addr;}
    void setName(const QString& name) {m_Name=name;}
private:
    QString m_Name;
    CAddr m_Addr;
};


#endif
