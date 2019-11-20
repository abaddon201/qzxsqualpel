#include <QDebug>
#include "qt_debug_printers.h"

#include "helper_widgets.h"

#include "disassembler_widget.h"

#include <QFile>
#include <QTextTable>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QPainter>

#include <memory>

#include "main_window.h"
#include "widget_change_text.h"
#include "gui_text_block_user_data.h"
#include "guichunk.h"
#include "core/debug_printers.h"

DisassemblerWidget::DisassemblerWidget()
    : QPlainTextEdit() {
  init();
}

DisassemblerWidget::DisassemblerWidget(MainWindow *mwnd)
    : QPlainTextEdit(), _main_window(mwnd) {

  init();
}

void DisassemblerWidget::init() {
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);
  setFont(font);

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

  _references_on_line = 3;
  setMinimumWidth(600);
}

void DisassemblerWidget::navigateToAddress(const Addr &addr) {
  qDebug() << "GUI: navigate to address:" << addr.toString();
  std::shared_ptr<GUIChunk> chunk = _chunks.getChunkContains(addr);
  if (nullptr != chunk) {
    QTextCursor cursor(textCursor());
    cursor.setPosition(chunk->cursorStartPosition() + 1);
    setTextCursor(cursor);
  }
  centerCursor();
}

void DisassemblerWidget::commentCommandUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: commentCommand:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  std::shared_ptr<GUIChunk> chunk = _chunks.getChunkByPosition(cursor.position());
  if (nullptr == chunk) {
    return;
  }
  GUITextBlockUserData *ud = (GUITextBlockUserData *) cursor.block().userData();
  if (!ud) {
    return;
  }
  Command *cmd = ud->cmd;
  if (cmd) {
    WidgetChangeText dlg(this, tr("Change command comment"), tr("Comment:"), QString::fromStdString(cmd->comment));
    if (dlg.exec()) {
      cmd->comment = dlg.text().toStdString();
      refreshView();
      navigateToAddress(chunk->core()->addr());
    }
  }
}

void DisassemblerWidget::changeNameUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: changeName:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  std::shared_ptr<GUIChunk> chunk = _chunks.getChunkByPosition(cursor.position());
  if (nullptr == chunk) {
    return;
  }
  if ((chunk->core()->type() == Chunk::Type::CODE)
      || (chunk->core()->type() == Chunk::Type::DATA_BYTE)
      || (chunk->core()->type() == Chunk::Type::DATA_ARRAY)
      || (chunk->core()->type() == Chunk::Type::DATA_WORD)
      ) {
    WidgetChangeText dlg(this, tr("Change label name"), tr("Label:"),
                         QString::fromStdString(chunk->core()->label()->name));
    if (dlg.exec()) {
      _disassembler_core->labels().changeLabel(chunk->core()->addr(), dlg.text().toStdString());
      refreshView();
      navigateToAddress(chunk->core()->addr());
    }
  }
}

void DisassemblerWidget::makeCodeUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: make code:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  std::shared_ptr<GUIChunk> chunk = _chunks.getChunkByPosition(cursor.position());
  if (nullptr == chunk) {
    return;
  }
  if (chunk->core()->isEmpty()) {
    Addr ret_addr = chunk->core()->addr();
    _disassembler_core->disassembleBlock(chunk->core()->addr());
    refreshView();
    navigateToAddress(ret_addr);
  }
}

void DisassemblerWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_C:
      // must codefi under cursor
      makeCodeUnderCursor();
      return;
    case Qt::Key_D:
      // must datefi under cursor
      return;
    case Qt::Key_U:
      // must uncode and undatefy under cursor
      return;
    case Qt::Key_N:
      // must set name under cursor
      changeNameUnderCursor();
      return;
    case Qt::Key_O:
      // must set offset under cursor
      return;
    case Qt::Key_Semicolon:
      // must set comment for command under cursor
      commentCommandUnderCursor();
      return;
    case Qt::Key_G:
      // jump to addr
    {
      NavigateToAddrDlg a{this};
      a();
      return;
    }
    case Qt::Key_Enter:
      // follow
      return;
    case Qt::Key_Escape:
      // follow back
      return;
  }
  //PageUp, PageDown, Up, Down, Left, and Right
  if ((event->key() == Qt::Key_PageUp)
      || (event->key() == Qt::Key_PageDown)
      || (event->key() == Qt::Key_Home)
      || (event->key() == Qt::Key_End)
      || (event->key() == Qt::Key_Up)
      || (event->key() == Qt::Key_Down)
      || (event->key() == Qt::Key_Left)
      || (event->key() == Qt::Key_Right))
    QPlainTextEdit::keyPressEvent(event);
}

void DisassemblerWidget::paintEvent(QPaintEvent *event) {
  QPlainTextEdit::paintEvent(event);
  QPainter p(viewport());
  QRect cr = cursorRect();
  cr.setRight(cr.right() + 5);
  p.fillRect(cr, QBrush(QColor(0, 100, 255, 120)));
}

void DisassemblerWidget::openRAWFile(const QString &fileName) {
  auto *buf = new unsigned char[65536];
  size_t loaded;
  FILE *f = fopen(fileName.toLocal8Bit(), "rb");
  loaded = fread(buf, 1, 65536, f);
  fclose(f);
  _disassembler_core->setRawMemory(buf, loaded);
  delete[] buf;
  ///@fixme: Add checks
  _disassembler_core->initialParse();
}

void DisassemblerWidget::saveASMFile(const QString &fileName) {
#if QT_VERSION >= 0x040500
  QTextDocumentWriter writer(fileName);
  writer.setFormat("plaintext");
  writer.write(document());
#else
  QFile f(fileName);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  out<<document()->toPlainText();
  f.close();
#endif
}

void
DisassemblerWidget::printCell(QTextCursor &cursor, const std::string &text, int length, const QTextCharFormat &fmt) {
  int spclen = length - text.length();
  if (spclen > 0) {
    std::string spcline(spclen, ' ');
    cursor.insertText(QString::fromStdString(text + spcline), fmt);
  } else {
    cursor.insertText(QString::fromStdString(text), fmt);
  }
}

void DisassemblerWidget::printCell(QTextCursor &cursor, const std::string &text, int length) {
  int spclen = length - text.length();
  if (spclen < 0) {
    spclen = 0;
  }
  std::string spcline(spclen, ' ');
  cursor.insertText(QString::fromStdString(text + spcline));
}

void DisassemblerWidget::printReferences(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  if (chunk->core()->references().size() == 0) {
    return;
  }
  int skip_len = _cell_length_command + _cell_length_args;
  int skip_len2 =
      _cell_length_addr + _cell_length_opcodes + _cell_length_label + _cell_length_command + _cell_length_args;

  int l = 0;
  for (auto ref: chunk->core()->references()) {
    if (l == 0) {
      printCell(cursor, std::string(), skip_len);
      l = 1;
    } else {
      printCell(cursor, std::string(), skip_len2);
    }
    printCell(cursor, ref.toString(), _cell_length_reference, _cell_format_reference);
    cursor.insertText("\n");
  }
}

void DisassemblerWidget::printCommand(QTextCursor &cursor, const Command &cmd) {
  cursor.insertBlock();
  if (cmd.addr<16384) {
    printCell(cursor, cmd.addr.toString(), _cell_length_addr, _cell_format_addr_rom);
  } else {
    printCell(cursor, cmd.addr.toString(), _cell_length_addr, _cell_format_addr);
  }
  printCell(cursor, cmd.getOpcodesString(), _cell_length_opcodes, _cell_format_opcodes);
  printCell(cursor, std::string(), _cell_length_label, _cell_format_label);
  printCell(cursor, cmd.command, _cell_length_command, _cell_format_command);
  printCell(cursor, cmd.getArgsString(), _cell_length_args, _cell_format_args);
  if (!cmd.comment.empty()) {
    printCell(cursor, std::string(";") + cmd.comment, _cell_length_cmd_comment, _cell_format_cmd_comment);
  } else if (!cmd.auto_comment.empty()) {
    printCell(cursor, std::string(";") + cmd.auto_comment, _cell_length_cmd_comment, _cell_format_cmd_auto_comment);
  }
}

void DisassemblerWidget::printChunkUnparsed(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  printCommand(cursor, chunk->core()->getCommand(0));
///@bug В профайлере - это самая дорогая операция... Стоит пересмотеть способ вывода
  //cursor.movePosition(QTextCursor::End);
}

void DisassemblerWidget::printChunkCode(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  if (chunk->core()->label() != nullptr) {
    cursor.insertBlock();
    if (!chunk->core()->comment().empty()) {
      printCell(cursor, chunk->core()->addr().toString(), _cell_length_addr, _cell_format_addr);
      printCell(cursor, std::string(), _cell_length_opcodes, _cell_format_opcodes);
      printCell(cursor, std::string(";") + chunk->core()->comment(), _cell_length_label, _cell_format_chunk_comment);
    }
    printCell(cursor, chunk->core()->addr().toString(), _cell_length_addr, _cell_format_addr);
    printCell(cursor, std::string(), _cell_length_opcodes, _cell_format_opcodes);
    printCell(cursor, chunk->core()->label()->name + ":", _cell_length_label, _cell_format_label);
    printReferences(cursor, chunk);
  }
  for (Command &cmd: chunk->core()->commands()) {
    printCommand(cursor, cmd);
    cursor.block().setUserData(new GUITextBlockUserData(chunk->core(), &cmd));
//    cursor.movePosition(QTextCursor::End);
  }
//  cursor.movePosition(QTextCursor::End);
}

void DisassemblerWidget::refreshView() {
  _chunks.update(_disassembler_core->chunks());
  clear();
  QTextCursor cursor(textCursor());
  cursor.beginEditBlock();
  int i = 0;
  for (auto &chunk: _chunks.chunks()) {
    chunk->setCursorStartPosition(cursor.position());
    switch (chunk->core()->type()) {
      case Chunk::Type::UNPARSED:
        printChunkUnparsed(cursor, chunk);
        break;
      case Chunk::Type::CODE:
        printChunkCode(cursor, chunk);
        break;
    };
    chunk->setCursorEndPosition(cursor.position());
    i++;
  }
  cursor.endEditBlock();
  _main_window->labelsWidget()->refresh();
}
