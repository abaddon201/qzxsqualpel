//
// C++ Interface: CCommand
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <QList>
#include <QString>

#include "CByte.h"
#include "CAddr.h"

class CCommand {
public:
    typedef enum {
        ARG_UNKNOWN=0,
        ARG_REGISTER=1,
        ARG_DATAOFFSET=2,
        ARG_JUMPOFFSET=3,
        ARG_VALUE=4,
        ARG_VALOFFSET=5,
        ARG_FLAG=6
    } ArgType;
    QList<CByte> opcodes;
    QString command;
    QString arg1;
    QString arg2;
public:
    CCommand() {}
    CCommand(const CCommand& c) {
        m_Addr=c.m_Addr;
        opcodes=c.opcodes;
        command=c.command;
        arg1=c.arg1;
        arg2=c.arg2;
        m_Comment=c.m_Comment;
    }
    CCommand& operator=(const CCommand& c) {
        m_Addr=c.m_Addr;
        opcodes=c.opcodes;
        command=c.command;
        arg1=c.arg1;
        arg2=c.arg2;
        m_Comment=c.m_Comment;
        return *this;
    }
    QString getArgsString() const;
    QString getOpcodesString() const;
    CAddr getJmpAddr() const;
    const CAddr& addr() const {
        return m_Addr;
    }
    void setAddr(const CAddr& addr) {
        m_Addr=addr;
    }
    QString comment() const {
        return m_Comment;
    }
private:
    QString m_Comment;
    CAddr m_Addr;

};

#endif
