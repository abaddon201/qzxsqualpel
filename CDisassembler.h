#ifndef CDISASSEMBLER_H
#define CDISASSEMBLER_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QPlainTextEdit>

#include "CChunkList.h"
#include "CLabel.h"
#include "CLabels.h"
#include "IDisassemblerCore.h"

class CMainWindow;

class CDisassembler : public QPlainTextEdit {
public:
    CDisassembler(CMainWindow* mwnd);
    void setCore(IDisassemblerCore* core) {
        m_DisassemblerCore=core;
    }
    void openRAWFile(QString fileName);
    void saveASMFile(QString fileName);
    void keyPressEvent ( QKeyEvent * );
    static CDisassembler* inst() {
        if (m_Inst==0) {
            //new CDisassembler;
            throw int(666);
        }
        return m_Inst;
    }

    CByte getMemoryByte(CAddr addr);

    int disassembleInstruction(CAddr addr);
    void disassembleBlock(CAddr addr);
    CChunk* createChunk(CAddr addr, CChunk::Type type=CChunk::Type::UNKNOWN);
    CLabels labels() {return m_Labels;}
    bool labelPresent(CAddr addr);
    CChunkList& chunks() {return m_Chunks;}
private:
    CDisassembler();
    void init();
    bool isChunkEmpty(CChunk* chunk);
    void makeJump(CAddr from_addr, CAddr jump_addr, CReference::ReferenceType ref_type);
    void initialParse();

    void printCell(QTextCursor& cursor, QString text, int length, QTextCharFormat fmt);
    void printCell(QTextCursor& cursor, QString text, int length);
    void printReferences(QTextCursor& cursor, CChunk* chunk);
    void printChunkUnparsed(QTextCursor& cursor, CChunk* chunk);
    void printChunkCode(QTextCursor& cursor, CChunk* chunk);
    void refreshView();

    void changeNameUnderCursor();
    void makeCodeUnderCursor();

    CByte m_MemoryPool[0xFFFF];
    int m_ProgLength;
    CChunkList m_Chunks;
//    QList<CChunk> m_Chunks;
    static CDisassembler* m_Inst;
    CLabels m_Labels;

    QTextCharFormat m_CellFormatAddr;
    QTextCharFormat m_CellFormatOpcodes;
    QTextCharFormat m_CellFormatLabel;
    QTextCharFormat m_CellFormatCommand;
    QTextCharFormat m_CellFormatArgs;
    QTextCharFormat m_CellFormatCmdComment;
    QTextCharFormat m_CellFormatChunkComment;
    QTextCharFormat m_CellFormatReference;

    int m_CellLengthAddr;
    int m_CellLengthOpcodes;
    int m_CellLengthLabel;
    int m_CellLengthCommand;
    int m_CellLengthArgs;
    int m_CellLengthCmdComment;
    int m_CellLengthReference;

    static const int CELLLENGTHADDR=7;
    static const int CELLLENGTHOPCODES=13;
    static const int CELLLENGTHLABEL=10;
    static const int CELLLENGTHCOMMAND=5;
    static const int CELLLENGTHARGS=10;
    static const int CELLLENGTHCMDCOMMENT=0;
    static const int CELLLENGTHREFERENCE=0;

    int m_ReferencesOnLine;

    CMainWindow* m_MainWindow;
    IDisassemblerCore* m_DisassemblerCore;
};

#endif
