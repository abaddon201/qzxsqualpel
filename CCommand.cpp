//
// C++ Implementation: CCommand
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "CCommand.h"

CAddr CCommand::getJmpAddr() const {
    if (arg2.isEmpty()) {
        //get from arg1
        return arg1.toInt(0, 16);
    } else {
        //get from arg2
        return arg2.toInt(0, 16);
    }
}

QString CCommand::getArgsString() const {
    if (arg1.isEmpty()) {
        return QString();
    }
    if (arg2.isEmpty()) {
        return arg1.toUpper();
    }
    return arg1.toUpper()+QString(", ")+arg2.toUpper();
}

QString CCommand::getOpcodesString() const {
    QString tmp;
    QString tmp2;
    foreach(CByte byte, opcodes) {
        tmp2=byte.toString();
        tmp+=tmp2+" ";
    }
    return tmp.toUpper();
}

