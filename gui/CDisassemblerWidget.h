#ifndef CDISASSEMBLER_H
#define CDISASSEMBLER_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QPlainTextEdit>

#include "core/CChunkList.h"
#include "core/CLabel.h"
#include "core/CLabels.h"
#include "core/IDisassemblerCore.h"

#include "guichunklist.h"

class CMainWindow;

class CDisassemblerWidget : public QPlainTextEdit {
public:
  CDisassemblerWidget(CMainWindow* mwnd);
  void setCore(IDisassemblerCore* core) {
    m_DisassemblerCore=core;
  }
  void openRAWFile(QString fileName);
  void saveASMFile(QString fileName);
  void keyPressEvent ( QKeyEvent* );

  ///@brief Показать на экране код с меткой под номером
  void navigateToLabel(CAddr& addr);

  void paintEvent(QPaintEvent* event);
  void refreshView();

private:
  CDisassemblerWidget();
  void init();

  void printCell(QTextCursor &cursor, std::string text, int length, QTextCharFormat fmt);
  void printCell(QTextCursor &cursor, std::string text, int length);

  void printReferences(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);
  void printChunkUnparsed(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);
  void printChunkCode(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);

  void changeNameUnderCursor();
  void makeCodeUnderCursor();

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
  IDisassemblerCore* m_DisassemblerCore;

  GUIChunkList _chunks;
};

#endif
