#include <QDebug>
#include "utils/qt_debug_printers.h"

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

#include "text_view_printer.h"

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

  dasm::gui::TextViewPrinter::init();
  dasm::gui::DocumentHelper::inst().init(this);
  _references_on_line = 3;
  setMinimumWidth(1200);
  setMinimumHeight(800);
}

dasm::core::CommandPtr DisassemblerWidget::getCmdUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: getCmdUnderCursor: " << cursor.block().text();
  qDebug() << "GUI: getCmdUnderCursor: Cursor pos:" << cursor.position();
  auto gcmd = _commands.getChunkByPosition(cursor.position());
  if (gcmd == nullptr) {
    return nullptr;
  }
  return gcmd->core();
}

DisassemblerWidget::GUICommandPtr DisassemblerWidget::getGuiCmdUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug() << "GUI: getGuiCmdUnderCursor: " << cursor.block().text();
  qDebug() << "GUI: getGuiCmdUnderCursor: Cursor pos:" << cursor.position();
  auto gcmd = _commands.getChunkByPosition(cursor.position());
  return gcmd;
}

void DisassemblerWidget::commentCommandUnderCursor() {
  auto cmd = getCmdUnderCursor();
  if (nullptr == cmd) {
    return;
  }
  WidgetChangeText dlg(this, tr("Change command comment"), tr("Comment:"), QString::fromStdString(cmd->comment));
  if (dlg.exec()) {
    cmd->comment = dlg.text().toStdString();
    refreshView();
    navigateToAddress(cmd->addr);
  }
}

void DisassemblerWidget::changeNameUnderCursor() {
  auto cmd = getCmdUnderCursor();
  if (nullptr == cmd) {
    return;
  }
  if (cmd->command_code != CmdCode::NONE) {
    WidgetChangeText dlg(this, tr("Change label name"), tr("Label:"), QString::fromStdString(cmd->label()->name));
    if (dlg.exec()) {
      dasm::core::DisassemblerCore::inst().labels().changeLabel(cmd->addr, dlg.text().toStdString());
      refreshView();
      navigateToAddress(cmd->addr);
    }
  }
}

void DisassemblerWidget::makeCodeUnderCursor() {
  auto cmd = getCmdUnderCursor();
  if (nullptr == cmd) {
    return;
  }
  if (cmd->command_code == dasm::core::CmdCode::NONE) {
    const auto& ret_addr = cmd->addr;
    dasm::core::DisassemblerCore::inst().disassembleBlock(ret_addr);
    refreshView();
    navigateToAddress(ret_addr);
  }
}

void DisassemblerWidget::makeArrayUnderCursor() {
  //make array
  auto cmd = getCmdUnderCursor();
  if (nullptr == cmd) {
    return;
  }
  if (cmd->command_code == CmdCode::NONE || cmd->command_code == CmdCode::DB || cmd->command_code == CmdCode::DW) {
    MakeArrayDlg a{ this };
    a();
  }
}

void DisassemblerWidget::makeArray(int size, bool clearMem) {
  auto cmd = getCmdUnderCursor();
  if (nullptr == cmd) {
    return;
  }
  if (cmd->command_code == CmdCode::NONE || cmd->command_code == CmdCode::DB || cmd->command_code == CmdCode::DW) {
    const auto& ret_addr = cmd->addr;
    dasm::core::DisassemblerCore::inst().makeArray(ret_addr, size, clearMem);
    refreshView();
    navigateToAddress(ret_addr);
  }
}

void DisassemblerWidget::navigateToAddress(const dasm::memory::Addr& from_addr, const dasm::memory::Addr& addr) {
  qDebug() << "GUI: navigate to address:" << addr.toString() << " from: " << from_addr.toString();
  dasm::gui::NavigationStack::inst().push(from_addr);
  navigateToAddress(addr);
}

void DisassemblerWidget::navigateToAddress(const dasm::memory::Addr& addr) {
  qDebug() << "GUI: navigate to address:" << addr.toString();
  auto cmd = getGuiCmdUnderCursor();
  if (nullptr != cmd) {
    QTextCursor cursor(textCursor());
    cursor.setPosition(cmd->cursorStartPosition() + 1);
    setTextCursor(cursor);
  }
  centerCursor();
}

void DisassemblerWidget::navigateToReference() {
  auto txt = dasm::gui::DocumentHelper::inst().getWordUnderCursor();
  dasm::memory::Addr addr;
  if (dasm::core::DisassemblerCore::inst().extractAddrFromRef(txt, addr)) {
    //navigating to addr
    qDebug() << "navigating to addr: " << addr.toString();
    try {
      navigateToAddress(dasm::gui::DocumentHelper::inst().getAddrFromLineStart(), addr);
    } catch (...) {
      std::cerr << "unable to navigate, source addr unknown" << std::endl;
    }
  } else {
    qDebug() << "Unable to parse!!!";
  }
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
  QTextDocumentWriter writer(fileName);
  writer.setFormat("plaintext");
  writer.write(document());
}

void DisassemblerWidget::refreshView() {
  _commands.clear();
  clear();
  QTextCursor cursor(textCursor());
  cursor.beginEditBlock();
  int i = 0;
  for (auto& cmd : dasm::core::DisassemblerCore::inst().commands().whole()) {
    if (cmd.offset() == 0) {
      auto gui_cmd = std::make_shared< GUIBlock<dasm::core::Command>>(cmd.elem());
      gui_cmd->setCursorStartPosition(cursor.position());
      _commands.push(gui_cmd);
      dasm::gui::TextViewPrinter::printCommand(cursor, cmd.elem());
      gui_cmd->setCursorEndPosition(cursor.position());
      cursor.insertText("\n");
    }
    i++;
  }
  cursor.endEditBlock();
  _main_window->labelsWidget()->refresh();
}
