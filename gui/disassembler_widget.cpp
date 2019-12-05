#include <QDebug>
#include "qt_debug_printers.h"

#include "navigate_to_addr_dlg.h"
#include "make_array_dlg.h"

#include "disassembler_widget.h"
#include "files/project.h"

#include <QFile>
#include <QTextTable>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QPainter>
#include <QMessageBox>

#include <memory>
#include <iostream>
#include <fstream>

#include "main_window.h"
#include "widget_change_text.h"
#include "guichunk.h"
#include "core/debug_printers.h"
#include "navigation_stack.h"

DisassemblerWidget::DisassemblerWidget()
  : QPlainTextEdit() {
  init();
}

DisassemblerWidget::DisassemblerWidget(MainWindow* mwnd)
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
  _cell_length_chunk_comment = CELL_LENGTH_CHUNK_COMMENT;

  _references_on_line = 3;
  setMinimumWidth(1200);
  setMinimumHeight(800);
}

void DisassemblerWidget::navigateToAddress(const dasm::memory::Addr& from_addr, const dasm::memory::Addr& addr) {
  qDebug() << "GUI: navigate to address:" << addr.toString() << " from: " << from_addr.toString();
  dasm::gui::NavigationStack::inst().push(from_addr);
  navigateToAddress(addr);
}

void DisassemblerWidget::navigateToAddress(const dasm::memory::Addr& addr) {
  qDebug() << "GUI: navigate to address:" << addr.toString();
  //auto chunk = _chunks.getChunkContains(addr);
  auto chunk = _commands.getChunkContains(addr);
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
  auto gcmd = _commands.getChunkByPosition(cursor.position());
  if (nullptr == gcmd) {
    return;
  }
  auto cmd = gcmd->core();
  /*GUITextBlockUserData* ud = (GUITextBlockUserData*)cursor.block().userData();
  if (!ud) {
    return;
  }
  CommandPtr cmd = ud->cmd;*/
  if (cmd) {
    WidgetChangeText dlg(this, tr("Change command comment"), tr("Comment:"), QString::fromStdString(cmd->comment));
    if (dlg.exec()) {
      cmd->comment = dlg.text().toStdString();
      refreshView();
      navigateToAddress(cmd->addr);
    }
  }
}

void DisassemblerWidget::changeNameUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: changeName:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  auto cmd = _commands.getChunkByPosition(cursor.position());
  if (nullptr == cmd) {
    return;
  }
  if (cmd->core()->command_code != CmdCode::NONE) {
    WidgetChangeText dlg(this, tr("Change label name"), tr("Label:"), QString::fromStdString(cmd->core()->label()->name));
    if (dlg.exec()) {
      dasm::core::DisassemblerCore::inst().labels().changeLabel(cmd->core()->addr, dlg.text().toStdString());
      refreshView();
      navigateToAddress(cmd->core()->addr);
    }
  }
}

void DisassemblerWidget::makeCodeUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: make code:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  auto chunk = _commands.getChunkByPosition(cursor.position());
  if (nullptr == chunk) {
    return;
  }
  if (chunk->core()->command_code == dasm::core::CmdCode::NONE) {
    const auto& ret_addr = chunk->core()->addr;
    dasm::core::DisassemblerCore::inst().disassembleBlock(ret_addr);
    refreshView();
    navigateToAddress(ret_addr);
  }
}

void DisassemblerWidget::makeArrayUnderCursor() {
  //make array
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: make array:" << cursor.block().text();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  auto cmd = _commands.getChunkByPosition(cursor.position());
  if (nullptr == cmd) {
    return;
  }
  if (cmd->core()->command_code==CmdCode::NONE || cmd->core()->command_code == CmdCode::DB || cmd->core()->command_code == CmdCode::DW) {
    MakeArrayDlg a{ this };
    a();
  }
}

void DisassemblerWidget::makeArray(int size, bool clearMem) {
  QTextCursor cursor(textCursor());
  auto cmd = _commands.getChunkByPosition(cursor.position());
  if (nullptr == cmd) {
    return;
  }
  if (cmd->core()->command_code == CmdCode::NONE || cmd->core()->command_code == CmdCode::DB || cmd->core()->command_code == CmdCode::DW) {
    const auto& ret_addr = cmd->core()->addr;
    dasm::core::DisassemblerCore::inst().makeArray(ret_addr, size, clearMem);
    refreshView();
    navigateToAddress(ret_addr);
  }
}

void DisassemblerWidget::navigateToReference() {
  QTextCursor cursor(textCursor());
  cursor.select(QTextCursor::WordUnderCursor);
  auto txt = cursor.selectedText();
  auto sel_strt = cursor.selectionStart() - 1;
  auto sel_end = cursor.selectionEnd();
  while (sel_strt >= 0 && !document()->characterAt(sel_strt).isSpace()) {
    //check syms left
    auto c = document()->characterAt(sel_strt);
    txt = c + txt;
    sel_strt--;
  }
  auto fin = sel_end + 30;
  while (sel_end < fin && !document()->characterAt(sel_end).isSpace()) {
    //check syms left
    auto c = document()->characterAt(sel_end);
    txt += c;
    ++sel_end;
  }
  qDebug() << "GUI: navigate to reference:" << cursor.block().text();
  qDebug() << "GUI: text:" << txt.toStdString();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  dasm::memory::Addr addr;
  if (dasm::core::DisassemblerCore::inst().extractAddrFromRef(txt.toStdString(), addr)) {
    //navigating to addr
    qDebug() << "navigating to addr: " << addr.toString();
    try {
      navigateToAddress(getCurrentAddr(), addr);
    } catch (...) {
      std::cerr << "unable to navigate, source addr unknown" << std::endl;
    }
  } else {
    qDebug() << "Unable to parse!!!";
  }
}

std::string DisassemblerWidget::getString(int pos, int count) const {
  QString res{};
  for (int i = 0; i < count; ++i) {
    res += document()->characterAt(pos + i);
  }
  return res.toStdString();
}

dasm::memory::Addr DisassemblerWidget::getCurrentAddr() const {
  QTextCursor cursor(textCursor());
  do {
    cursor.movePosition(QTextCursor::StartOfLine);
    auto pos = cursor.position();
    std::string segs = getString(pos, 4);
    std::string offss = getString(pos + 5, 4);
    try {
      auto seg = dasm::utils::hex2int(segs);
      auto offs = dasm::utils::hex2int(offss);
      return dasm::memory::Addr(offs, seg);
    } catch (...) {
      //unable to decode
    }
    cursor.movePosition(QTextCursor::Up);
  } while (cursor.position() != 0);
  throw;
}

void DisassemblerWidget::keyPressEvent(QKeyEvent* event) {
  //std::cout << "key: " << std::to_string(event->key()) << std::endl;
  switch (event->key()) {
    case Qt::Key_C:
      // must codefi under cursor
      makeCodeUnderCursor();
      return;
    case Qt::Key_D:
      // must datefi under cursor
      return;
    case Qt::Key_Asterisk:
      makeArrayUnderCursor();
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
      NavigateToAddrDlg a{ this };
      a();
      return;
    }
    case Qt::Key_Enter:
    case Qt::Key_Return:
      // follow
      navigateToReference();
      return;
    case Qt::Key_Escape:
      // follow back
      if (dasm::gui::NavigationStack::inst().hasAddr()) {
        navigateToAddress(dasm::gui::NavigationStack::inst().pop());
      }
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

void DisassemblerWidget::paintEvent(QPaintEvent* event) {
  QPlainTextEdit::paintEvent(event);
  QPainter p(viewport());
  QRect cr = cursorRect();
  cr.setRight(cr.right() + 5);
  p.fillRect(cr, QBrush(QColor(0, 100, 255, 120)));
}

void DisassemblerWidget::openRAWFile(const QString& fileName) {
  auto* buf = new unsigned char[65536];
  size_t loaded;
  FILE* f = fopen(fileName.toLocal8Bit(), "rb");
  loaded = fread(buf, 1, 65536, f);
  fclose(f);
  dasm::core::DisassemblerCore::inst().setRawMemory(buf, loaded);
  dasm::core::DisassemblerCore::inst().setFileName(fileName.toStdString());
  delete[] buf;
  ///@fixme: Add checks
  dasm::core::DisassemblerCore::inst().initialParse();
}

void DisassemblerWidget::saveProjectFile(const QString& fileName) {
  std::ofstream file;
  file.open(fileName.toStdString());
  if (!file.is_open()) {
    QMessageBox::information(this, tr("Unable to open file"), fileName);
    return;
  }
  std::string res = dasm::files::project::Serializer::serialize(dasm::core::DisassemblerCore::inst());
  file << res;
  file.close();
}

void DisassemblerWidget::openProjectFile(const QString& fileName) {
  dasm::files::project::Serializer::deserialize_file(fileName.toStdString(), dasm::core::DisassemblerCore::inst());
  refreshView();
}

void DisassemblerWidget::saveASMFile(const QString& fileName) {
#if QT_VERSION >= 0x040500
  QTextDocumentWriter writer(fileName);
  writer.setFormat("plaintext");
  writer.write(document());
#else
  QFile f(fileName);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  out << document()->toPlainText();
  f.close();
#endif
}

void
DisassemblerWidget::printCell(QTextCursor& cursor, const std::string& text, int length, const QTextCharFormat& fmt) {
  int spclen = length - (int)text.length();
  if (spclen > 0) {
    std::string spcline(spclen, ' ');
    cursor.insertText(QString::fromStdString(text + spcline), fmt);
  } else {
    cursor.insertText(QString::fromStdString(text), fmt);
  }
}

void DisassemblerWidget::printCell(QTextCursor& cursor, const std::string& text, int length) {
  int spclen = length - (int)text.length();
  if (spclen < 0) {
    spclen = 0;
  }
  std::string spcline(spclen, ' ');
  cursor.insertText(QString::fromStdString(text + spcline));
}

void DisassemblerWidget::printReferences(QTextCursor& cursor, dasm::core::CommandPtr cmd) {
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

void DisassemblerWidget::printCommand(QTextCursor& cursor, const CommandPtr cmd) {
  //cursor.insertBlock();
  if (cmd->addr < 16384) {
    printCell(cursor, cmd->addr.toString(), _cell_length_addr, _cell_format_addr_rom);
  } else {
    printCell(cursor, cmd->addr.toString(), _cell_length_addr, _cell_format_addr);
  }
  printCell(cursor, cmd->getOpcodesString(MAX_OPCODES_COUNT), _cell_length_opcodes, _cell_format_opcodes);
  //printCell(cursor, std::string(), _cell_length_label, _cell_format_label);
  printCell(cursor, cmd->command_code.toString(), _cell_length_command, _cell_format_command);
  printCell(cursor, cmd->getArgsString(), _cell_length_args, _cell_format_args);
  if (!cmd->comment.empty()) {
    printCell(cursor, std::string(";") + cmd->comment, _cell_length_cmd_comment, _cell_format_cmd_comment);
  } else if (!cmd->auto_comment.empty()) {
    printCell(cursor, std::string(";") + cmd->auto_comment, _cell_length_cmd_comment, _cell_format_cmd_auto_comment);
  }
}

void DisassemblerWidget::printChunkCode(QTextCursor& cursor, dasm::core::CommandPtr cmd) {
  if (cmd->label() != nullptr) {
    //cursor.insertBlock();
    if (!cmd->blockComment().empty()) {
      printCell(cursor, cmd->addr.toString(), _cell_length_addr, _cell_format_addr);
      //printCell(cursor, std::string(), _cell_length_opcodes, _cell_format_opcodes);
      printCell(cursor, std::string(";") + cmd->blockComment(), _cell_length_chunk_comment, _cell_format_chunk_comment);
      //cursor.insertBlock();
    }
    printCell(cursor, cmd->addr.toString(), _cell_length_addr, _cell_format_addr);
    //printCell(cursor, std::string(), _cell_length_opcodes, _cell_format_opcodes);
    printCell(cursor, cmd->label()->name + ":", _cell_length_label, _cell_format_label);
    printReferences(cursor, cmd);
    cursor.insertText("\n");
  }
  //for (CommandPtr cmd : chunk->commands()) {
    //std::cout << "cur_pos: " << cursor.position() << std::endl;
    auto gui_cmd = std::make_shared< GUIBlock<dasm::core::Command>>(cmd);
    gui_cmd->setCursorStartPosition(cursor.position());
    _commands.push(gui_cmd);
    printCommand(cursor, cmd);
    gui_cmd->setCursorEndPosition(cursor.position());
    cursor.insertText("\n");
    //cursor.block().setUserData(new GUITextBlockUserData(chunk, cmd));
    //    cursor.movePosition(QTextCursor::End);
  //}
  //  cursor.movePosition(QTextCursor::End);
}

void DisassemblerWidget::refreshView() {
  _commands.clear();
  clear();
  QTextCursor cursor(textCursor());
  cursor.beginEditBlock();
  int i = 0;
  for (auto& cmd : dasm::core::DisassemblerCore::inst().commands().whole()) {
    if (cmd.offset() == 0) {
      printChunkCode(cursor, cmd.elem());
    }
    i++;
  }
  cursor.endEditBlock();
  _main_window->labelsWidget()->refresh();
}
