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

class MainWindow;

class DisassemblerWidget : public QPlainTextEdit {
public:
  DisassemblerWidget(MainWindow* mwnd);
  void setCore(IDisassemblerCore* core) {
    _disassembler_core=core;
  }
  void openRAWFile(QString fileName);
  void saveASMFile(QString fileName);
  void keyPressEvent ( QKeyEvent* );

  ///@brief Показать на экране код с меткой под номером
  void navigateToAddress(const Addr &addr);

  void paintEvent(QPaintEvent* event);
  void refreshView();

private:
  DisassemblerWidget();
  void init();

  void printCell(QTextCursor &cursor, std::string text, int length, QTextCharFormat fmt);
  void printCell(QTextCursor &cursor, std::string text, int length);

  void printReferences(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);
  void printCommand(QTextCursor &cursor, const Command &cmd);
  void __attribute__ ((deprecated)) printChunkUnparsed(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);
  void printChunkCode(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk);

  void changeNameUnderCursor();
  void makeCodeUnderCursor();

  void navigateToAddrDlg();

  QTextCharFormat _cell_format_addr;
  QTextCharFormat _cell_format_opcodes;
  QTextCharFormat _cell_format_label;
  QTextCharFormat _cell_format_command;
  QTextCharFormat _cell_format_args;
  QTextCharFormat _cell_format_cmd_comment;
  QTextCharFormat _cell_format_cmd_auto_comment;
  QTextCharFormat _cell_format_chunk_comment;
  QTextCharFormat _cell_format_reference;

  int _cell_length_addr;
  int _cell_length_opcodes;
  int _cell_length_label;
  int _cell_length_command;
  int _cell_length_args;
  int _cell_length_cmd_comment;
  int _cell_length_reference;

  static const int CELL_LENGTH_ADDR = 7;
  static const int CELL_LENGTH_OPCODES = 13;
  static const int CELL_LENGTH_LABEL = 10;
  static const int CELL_LENGTH_COMMAND = 5;
  static const int CELL_LENGTH_ARGS = 10;
  static const int CELL_LENGTH_CMD_COMMENT = 0;
  static const int CELL_LENGTH_REFERENCE = 0;

  int _references_on_line;

  MainWindow* _main_window;
  IDisassemblerCore* _disassembler_core;

  GUIChunkList _chunks;
};

#endif
