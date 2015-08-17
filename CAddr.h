//
// C++ Interface: CAddr
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CADDR_H
#define CADDR_H

#include <QString>

class CAddr {
public:
    CAddr(unsigned offs=0, unsigned seg=0) : m_Offset(offs), m_Segment(seg) {}
    bool operator==(const CAddr& s) const {
        if ((m_Offset==s.m_Offset) && (m_Segment==s.m_Segment)) {
            return true;
        }
        return false;
    }
    bool operator!=(const CAddr& s) const {
        if ((m_Offset!=s.m_Offset) || (m_Segment!=s.m_Segment)) {
            return true;
        }
        return false;
    }
    bool operator>=(const CAddr& s) const {
        if ((m_Offset>=s.m_Offset) && (m_Segment==s.m_Segment)) {
            return true;
        }
        return false;
    }
    bool operator<=(const CAddr& s) const {
        if ((m_Offset<=s.m_Offset) && (m_Segment==s.m_Segment)) {
            return true;
        }
        return false;
    }
    bool operator>(const CAddr& s) const {
        if ((m_Offset>s.m_Offset) && (m_Segment==s.m_Segment)) {
            return true;
        }
        return false;
    }
    bool operator<(const CAddr& s) const {
        if ((m_Offset>s.m_Offset) && (m_Segment==s.m_Segment)) {
            return true;
        }
        return false;
    }
    CAddr& operator++() {
        m_Offset++;
        return*this;
    }
    CAddr& operator--() {
        m_Offset--;
        return*this;
    }
    CAddr operator+(unsigned offs) {
        return m_Offset+offs;
    }
    CAddr operator-(const CAddr& raddr) {
        return m_Offset-raddr.m_Offset;
    }
    CAddr& operator+=(unsigned offs) {
        m_Offset+=offs;
        return *this;
    }
    unsigned offset() const {
        return m_Offset;
    }
    QString toString() const;
    QString offsetString() const;
private:
    unsigned m_Offset;
    unsigned m_Segment;
};

#endif
