#ifndef CDISASSEMBLER_H
#define CDISASSEMBLER_H

#include <QString>
#include <QByteArray>
#include <QList>
#include <QPlainTextEdit>

#include "core/chunk_list.h"
#include "core/label.h"
#include "core/labels.h"
#include "core/disassembler_core.h"

#include "guichunklist.h"

class MainWindow;

class DisassemblerWidget : public QPlainTextEdit {
public:
  DisassemblerWidget(MainWindow* mwnd);

  void openRAWFile(const QString& fileName);
  void saveProjectFile(const QString& fileName);
  void saveASMFile(const QString& fileName);
  void keyPressEvent(QKeyEvent*);

  ///@brief Показать на экране код с меткой под номером
  void navigateToAddress(const dasm::memory::Addr& addr);

  void makeArray(int size, bool clearMem);

  void paintEvent(QPaintEvent* event);
  void refreshView();

public slots:
  void changeNameUnderCursor();
  void makeCodeUnderCursor();
  void makeArrayUnderCursor();
  void commentCommandUnderCursor();

private:
  DisassemblerWidget();
  void init();

  void printCell(QTextCursor& cursor, const std::string& text, int length, const QTextCharFormat& fmt);
  void printCell(QTextCursor& cursor, const std::string& text, int length);

  void printReferences(QTextCursor& cursor, std::shared_ptr<GUIChunk> chunk);
  void printCommand(QTextCursor& cursor, const dasm::core::Command& cmd);
  //C++14 atributes -- http://en.cppreference.com/w/cpp/language/attributes
  [[deprecated]] void printChunkUnparsed(QTextCursor& cursor, std::shared_ptr<GUIChunk> chunk);
  void printChunkCode(QTextCursor& cursor, std::shared_ptr<GUIChunk> chunk);
  void printChunkData(QTextCursor& cursor, std::shared_ptr<GUIChunk> chunk);

  void navigateToAddrDlg();

  QTextCharFormat _cell_format_addr;
  QTextCharFormat _cell_format_addr_rom;
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
  int _cell_length_chunk_comment;
  int _cell_length_reference;

  static const int CELL_LENGTH_ADDR = 10;
  static const int CELL_LENGTH_OPCODES = 19;
  static const int CELL_LENGTH_LABEL = 19;
  static const int CELL_LENGTH_COMMAND = 5;
  static const int CELL_LENGTH_ARGS = 10;
  static const int CELL_LENGTH_CHUNK_COMMENT = 0;
  static const int CELL_LENGTH_CMD_COMMENT = 0;
  static const int CELL_LENGTH_REFERENCE = 0;
  static const size_t MAX_OPCODES_COUNT = 5;

  int _references_on_line;

  MainWindow* _main_window;

  GUIChunkList _chunks;
  Q_OBJECT
};

#endif
