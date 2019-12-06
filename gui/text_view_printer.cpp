#include "text_view_printer.h"

namespace dasm {
namespace gui {

void TextViewPrinter::init() {
  _cell_format_addr.setForeground(QColor("#000000"));
  _cell_format_addr_rom.setForeground(QColor("#7f7f7f"));
  _cell_format_opcodes.setForeground(QColor("#004000"));
  _cell_format_label.setForeground(QColor("#d00000"));
  _cell_format_command.setForeground(QColor("#0000d0"));
  _cell_format_args.setForeground(QColor("#000000"));
  _cell_format_cmd_comment.setForeground(QColor("#909090"));
  _cell_format_cmd_auto_comment.setForeground(QColor("#109090"));
  _cell_format_chunk_comment.setForeground(QColor("#909090"));
  _cell_format_reference.setForeground(QColor("#9090ff"));

  _cell_length_addr = CELL_LENGTH_ADDR;
  _cell_length_opcodes = CELL_LENGTH_OPCODES;
  _cell_length_label = CELL_LENGTH_LABEL;
  _cell_length_command = CELL_LENGTH_COMMAND;
  _cell_length_args = CELL_LENGTH_ARGS;
  _cell_length_cmd_comment = CELL_LENGTH_CMD_COMMENT;
  _cell_length_reference = CELL_LENGTH_REFERENCE;
  _cell_length_chunk_comment = CELL_LENGTH_CHUNK_COMMENT;
}

void TextViewPrinter::printCell(QTextCursor& cursor, const std::string& text, int length, const QTextCharFormat& fmt) {
  int spclen = length - (int)text.length();
  if (spclen > 0) {
    std::string spcline(spclen, ' ');
    cursor.insertText(QString::fromStdString(text + spcline), fmt);
  } else {
    cursor.insertText(QString::fromStdString(text), fmt);
  }
}

void TextViewPrinter::printCell(QTextCursor& cursor, const std::string& text, int length) {
  int spclen = length - (int)text.length();
  if (spclen < 0) {
    spclen = 0;
  }
  std::string spcline(spclen, ' ');
  cursor.insertText(QString::fromStdString(text + spcline));
}

void TextViewPrinter::printReferences(QTextCursor& cursor, dasm::core::CommandPtr cmd) {
  if (cmd->references().size() == 0) {
    return;
  }
  bool is_first = true;
  std::string empty{ "" };
  for (auto& ref : cmd->references()) {
    if (is_first) {
      is_first = false;
    } else {
      cursor.insertText("\n");
      printCell(cursor, empty, _cell_length_addr + _cell_length_label, _cell_format_addr_rom);
    }
    printCell(cursor, ref->toString(), _cell_length_reference, _cell_format_reference);
  }
}

void TextViewPrinter::printCommand(QTextCursor& cursor, const core::CommandPtr cmd) {
  if (!cmd->blockComment().empty()) {
    printCell(cursor, std::string(";") + cmd->blockComment(), _cell_length_chunk_comment, _cell_format_chunk_comment);
    cursor.insertText("\n");
  }
  if (cmd->label() != nullptr) {
    printCell(cursor, utils::toHex(cmd->addr), _cell_length_addr, _cell_format_addr);
    printCell(cursor, cmd->label()->name + ":", _cell_length_label, _cell_format_label);
    printReferences(cursor, cmd);
    cursor.insertText("\n");
  }
  if (cmd->addr < 16384) {
    printCell(cursor, utils::toHex(cmd->addr), _cell_length_addr, _cell_format_addr_rom);
  } else {
    printCell(cursor, utils::toHex(cmd->addr), _cell_length_addr, _cell_format_addr);
  }
  printCell(cursor, cmd->getOpcodesString(MAX_OPCODES_COUNT), _cell_length_opcodes, _cell_format_opcodes);
  printCell(cursor, cmd->command_code.toString(), _cell_length_command, _cell_format_command);
  printCell(cursor, cmd->getArgsString(), _cell_length_args, _cell_format_args);
  if (!cmd->comment.empty()) {
    printCell(cursor, std::string(";") + cmd->comment, _cell_length_cmd_comment, _cell_format_cmd_comment);
  } else if (!cmd->auto_comment.empty()) {
    printCell(cursor, std::string(";") + cmd->auto_comment, _cell_length_cmd_comment, _cell_format_cmd_auto_comment);
  }
}

QTextCharFormat TextViewPrinter::_cell_format_addr;
QTextCharFormat TextViewPrinter::_cell_format_addr_rom;
QTextCharFormat TextViewPrinter::_cell_format_opcodes;
QTextCharFormat TextViewPrinter::_cell_format_label;
QTextCharFormat TextViewPrinter::_cell_format_command;
QTextCharFormat TextViewPrinter::_cell_format_args;
QTextCharFormat TextViewPrinter::_cell_format_cmd_comment;
QTextCharFormat TextViewPrinter::_cell_format_cmd_auto_comment;
QTextCharFormat TextViewPrinter::_cell_format_chunk_comment;
QTextCharFormat TextViewPrinter::_cell_format_reference;

int TextViewPrinter::_cell_length_addr;
int TextViewPrinter::_cell_length_opcodes;
int TextViewPrinter::_cell_length_label;
int TextViewPrinter::_cell_length_command;
int TextViewPrinter::_cell_length_args;
int TextViewPrinter::_cell_length_cmd_comment;
int TextViewPrinter::_cell_length_chunk_comment;
int TextViewPrinter::_cell_length_reference;

}
}