//
// C++ Interface: CChunk
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CCHUNK_H
#define CCHUNK_H

#include "CReference.h"
#include "CCommand.h"

class CChunk {
public:
    typedef enum {
        UNKNOWN=-1,
        UNPARSED=0,
        CODE=1,
        DATA_BYTE=2,
        DATA_WORD=3,
        DATA_ARRAY=4
    } ChunkType;

    QString comment;
    QList<CReference> references;
public:
    CChunk(const CChunk& ch) {
        makeCopy(ch);
    }
    CChunk& operator=(const CChunk& ch) {
        makeCopy(ch);
        return *this;
    }
    CChunk* splitAt(CAddr addr);
    void addCrossRef(CAddr addr, CReference::ReferenceType type);
    void appendCommand(CCommand cmd);
    CCommand getCommand(int idx);
    CCommand lastCommand();
    int commandsCount() {
        return m_Commands.count();
    }
    QList<CCommand>& commands() {
        return m_Commands;
    }
    CAddr addr() {
        return m_StartingAddr;
    }
    QString label() {
        return m_Label;
    }
    ChunkType type() {
        return m_Type;
    }
    QString setLabel(QString label=QString(), CReference::ReferenceType=CReference::JUMP);
    void changeLabel(QString label) {m_Label=label;}
    void setCursorEndPosition(int pos) {m_EndCursorPosition=pos;}
    void setCursorStartPosition(int pos) {m_StartCursorPosition=pos;}
    int cursorEndPosition() {return m_EndCursorPosition;}
    int cursorStartPosition() {return m_StartCursorPosition;}
    ~CChunk() {};
private:
    friend class CChunkList;
    CChunk() : m_Length(0) {};
    CChunk(CAddr addr, CChunk::ChunkType type=CChunk::UNKNOWN) : m_Type(type), m_StartingAddr(addr) {};
    void makeCopy(const CChunk& ch) {
        m_StartingAddr=ch.m_StartingAddr;
        m_Length=ch.m_Length;
        comment=ch.comment;
        m_Commands=ch.m_Commands;
        references=ch.references;
        m_Label=ch.m_Label;
        m_Type=ch.m_Type;
        m_StartCursorPosition=ch.m_StartCursorPosition;
        m_EndCursorPosition=ch.m_EndCursorPosition;
    }

    QString m_Label;
    QList<CCommand> m_Commands;
    ChunkType m_Type;
    CAddr m_StartingAddr;
    int m_StartCursorPosition;
    int m_EndCursorPosition;
    unsigned m_Length;
};

#endif
