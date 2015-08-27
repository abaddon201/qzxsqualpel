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
  void keyPressEvent ( QKeyEvent* );

  static CDisassembler* inst() {
    if (m_Inst==0) {
      //new CDisassembler;
      throw int(666);
    }
    return m_Inst;
  }

  static IDisassemblerCore* core_inst() {
    if (m_DisassemblerCore==0) {
      throw int(667);
    }
    return m_DisassemblerCore;
  }

  ///@brief Показать на экране код с меткой под номером
  void navigateToLabel(int num);

  void paintEvent(QPaintEvent* event);
  void refreshView();

private:
  CDisassembler();
  void init();

  void printCell(QTextCursor &cursor, QString text, int length, QTextCharFormat fmt);
  void printCell(QTextCursor &cursor, QString text, int length);
  void printReferences(QTextCursor &cursor, CChunk* chunk);
  void printChunkUnparsed(QTextCursor &cursor, CChunk* chunk);
  void printChunkCode(QTextCursor &cursor, CChunk* chunk);

  void changeNameUnderCursor();
  void makeCodeUnderCursor();

  static CDisassembler* m_Inst;

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

  static const int CELL_LENGTH_ADDR = 7;
  static const int CELL_LENGTH_OPCODES = 13;
  static const int CELL_LENGTH_LABEL = 10;
  static const int CELL_LENGTH_COMMAND = 5;
  static const int CELL_LENGTH_ARGS = 10;
  static const int CELL_LENGTH_CMD_COMMENT = 0;
  static const int CELL_LENGTH_REFERENCE = 0;

  int m_ReferencesOnLine;

  CMainWindow* m_MainWindow;
  static IDisassemblerCore* m_DisassemblerCore;
};

#endif
