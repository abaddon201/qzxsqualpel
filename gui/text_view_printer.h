#pragma once

#include <QTextCursor>
#include <string>

#include "core/command.h"

namespace dasm {
namespace gui {

class TextViewPrinter {
public:

  static void init();

  static void removeBlock(QTextCursor& cursor, int length);

  static void printCell(QTextCursor& cursor, const std::string& text, int length, const QTextCharFormat& fmt);
  static void printCell(QTextCursor& cursor, const std::string& text, int length);

  static void printReferences(QTextCursor& cursor, const dasm::core::LabelPtr& cmd);
  static int printCommand(QTextCursor& cursor, const dasm::core::CommandPtr& cmd);

  static QTextCharFormat _cell_format_addr;
  static QTextCharFormat _cell_format_addr_rom;
  static QTextCharFormat _cell_format_opcodes;
  static QTextCharFormat _cell_format_label;
  static QTextCharFormat _cell_format_command;
  static QTextCharFormat _cell_format_args;
  static QTextCharFormat _cell_format_cmd_comment;
  static QTextCharFormat _cell_format_cmd_auto_comment;
  static QTextCharFormat _cell_format_block_comment;
  static QTextCharFormat _cell_format_reference;

  static int _cell_length_addr;
  static int _cell_length_opcodes;
  static int _cell_length_label;
  static int _cell_length_command;
  static int _cell_length_args;
  static int _cell_length_cmd_comment;
  static int _cell_length_block_comment;
  static int _cell_length_reference;

  static const int CELL_LENGTH_ADDR = 10;
  static const int CELL_LENGTH_OPCODES = 19;
  static const int CELL_LENGTH_LABEL = 19;
  static const int CELL_LENGTH_COMMAND = 5;
  static const int CELL_LENGTH_ARGS = 10;
  static const int CELL_LENGTH_BLOCK_COMMENT = 0;
  static const int CELL_LENGTH_CMD_COMMENT = 0;
  static const int CELL_LENGTH_REFERENCE = 0;
  static const size_t MAX_OPCODES_COUNT = 5;


};
}
}