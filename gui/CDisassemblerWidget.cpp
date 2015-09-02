#include <QDebug>

#include "CDisassemblerWidget.h"

#include <QFile>
#include <QTextTable>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QPainter>

#include <memory>

#include "CMainWindow.h"
#include "CWidgetChangeLabel.h"
#include "guichunk.h"
#include "core/debug_printers.h"

CDisassemblerWidget::CDisassemblerWidget()
  : QPlainTextEdit() {
  init();
}

CDisassemblerWidget::CDisassemblerWidget(CMainWindow* mwnd)
  : QPlainTextEdit()
  , m_MainWindow(mwnd) {

  init();
}

void CDisassemblerWidget::init() {
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);
  setFont(font);

  m_CellFormatAddr.setForeground(QColor("#000000"));
  m_CellFormatOpcodes.setForeground(QColor("#004000"));
  m_CellFormatLabel.setForeground(QColor("#d00000"));
  m_CellFormatCommand.setForeground(QColor("#0000d0"));
  m_CellFormatArgs.setForeground(QColor("#000000"));
  m_CellFormatCmdComment.setForeground(QColor("#909090"));
  m_CellFormatChunkComment.setForeground(QColor("#909090"));
  m_CellFormatReference.setForeground(QColor("#9090ff"));

  m_CellLengthAddr=CELL_LENGTH_ADDR;
  m_CellLengthOpcodes=CELL_LENGTH_OPCODES;
  m_CellLengthLabel=CELL_LENGTH_LABEL;
  m_CellLengthCommand=CELL_LENGTH_COMMAND;
  m_CellLengthArgs=CELL_LENGTH_ARGS;
  m_CellLengthCmdComment=CELL_LENGTH_CMD_COMMENT;
  m_CellLengthReference=CELL_LENGTH_REFERENCE;

  m_ReferencesOnLine=3;
}

void CDisassemblerWidget::navigateToLabel(CAddr &addr) {
  qDebug()<< "navigate to label:" << addr.toString();
  std::shared_ptr<GUIChunk> chunk = _chunks.getChunkContains(addr);
  if (nullptr!=chunk) {
    QTextCursor cursor(textCursor());
    cursor.setPosition(chunk->cursorStartPosition());
    setTextCursor(cursor);
  }
}

void CDisassemblerWidget::paintEvent(QPaintEvent* event) {
  QPlainTextEdit::paintEvent(event);
  QPainter p(viewport());
  QRect cr = cursorRect();
  cr.setRight(cr.right()+5);
  p.fillRect(cr, QBrush(QColor(0, 100, 255, 120)));
}

void CDisassemblerWidget::keyPressEvent(QKeyEvent* event) {
  switch (event->key()) {
  case Qt::Key_C:
    // must codefi under cursor
    makeCodeUnderCursor();
    return;
  case Qt::Key_D:
    // must datefi under cursor
    return;
  case Qt::Key_N:
    // must set name under cursor
    changeNameUnderCursor();
    return;
  case Qt::Key_O:
    // must set offset under cursor
    return;
  case Qt::Key_Enter:
    // follow
    return;
  case Qt::Key_Escape:
    // follow back
    return;
  }
  //PageUp, PageDown, Up, Down, Left, and Right
  if ((event->key()==Qt::Key_PageUp)
      || (event->key()==Qt::Key_PageDown)
      || (event->key()==Qt::Key_Up)
      || (event->key()==Qt::Key_Down)
      || (event->key()==Qt::Key_Left)
      || (event->key()==Qt::Key_Right))
    QPlainTextEdit::keyPressEvent(event);
}

void CDisassemblerWidget::changeNameUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  std::shared_ptr<GUIChunk> chunk=_chunks.getChunkByPosition(cursor.position());
  if (nullptr==chunk) {
    return;
  }
  if ((chunk->core()->type()==CChunk::Type::CODE)
      || (chunk->core()->type()==CChunk::Type::DATA_BYTE)
      || (chunk->core()->type()==CChunk::Type::DATA_ARRAY)
      || (chunk->core()->type()==CChunk::Type::DATA_WORD)
     ) {
    CWidgetChangeLabel dlg(this, QString::fromStdString(chunk->core()->label()));
    if (dlg.exec()) {
      m_DisassemblerCore->labels().changeLabel(chunk->core(), dlg.label().toStdString());
      refreshView();
    }
  }
}

void CDisassemblerWidget::makeCodeUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  std::shared_ptr<GUIChunk> chunk=_chunks.getChunkByPosition(cursor.position());
  if (nullptr==chunk) {
    return;
  }
  if (chunk->core()->isEmpty()) {
    m_DisassemblerCore->disassembleBlock(chunk->core()->addr());
    refreshView();
  }
}

void CDisassemblerWidget::openRAWFile(QString fileName) {
  unsigned char* buf = new unsigned char[65536];
  size_t loaded;
  FILE* f=fopen(fileName.toLocal8Bit(), "rb");
  loaded=fread(buf, 1, 65536, f);
  fclose(f);
  m_DisassemblerCore->setRawMemory(buf, loaded);
  delete[] buf;
  ///@fixme: Add checks
  m_DisassemblerCore->initialParse();
}

void CDisassemblerWidget::saveASMFile(QString fileName) {
#if QT_VERSION>=0x040500
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

void CDisassemblerWidget::printCell(QTextCursor &cursor, std::string text, int length, QTextCharFormat fmt) {
  int spclen=length-text.length();
  if (spclen>0) {
    std::string spcline(spclen, ' ');
    cursor.insertText(QString::fromStdString(text+spcline), fmt);
  } else {
    cursor.insertText(QString::fromStdString(text), fmt);
  }
}

void CDisassemblerWidget::printCell(QTextCursor &cursor, std::string text, int length) {
  int spclen=length-text.length();
  if (spclen<0) {
    spclen=0;
  }
  std::string spcline(spclen, ' ');
  cursor.insertText(QString::fromStdString(text+spcline));
}

void CDisassemblerWidget::printReferences(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  if (chunk->core()->references().size()==0) {
    return;
  }
  int skip_len=m_CellLengthCommand+m_CellLengthArgs;
  int skip_len2=m_CellLengthAddr+m_CellLengthOpcodes+m_CellLengthLabel+m_CellLengthCommand+m_CellLengthArgs;

  int l=0;
  for (auto ref: chunk->core()->references()) {
    if (l==0) {
      printCell(cursor, std::string(), skip_len);
      l=1;
    } else {
      printCell(cursor, std::string(), skip_len2);
    }
    /*    for (int i=0; i<m_ReferencesOnLine; i++) {
          CReference ref=it;*/
    printCell(cursor, ref.toString(), m_CellLengthReference, m_CellFormatReference);
    /*      ++it;
          if (it==chunk->core()->references().end()) {
            return;
          }
        }*/
    cursor.insertText("\n");
  }
}

void CDisassemblerWidget::printChunkUnparsed(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  cursor.insertBlock();
  CCommand cmd=chunk->core()->getCommand(0);
  printCell(cursor, cmd.addr.toString(), m_CellLengthAddr, m_CellFormatAddr);
  printCell(cursor, cmd.getOpcodesString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
  printCell(cursor, std::string(), m_CellLengthLabel, m_CellFormatLabel);
  printCell(cursor, cmd.command, m_CellLengthCommand, m_CellFormatCommand);
  printCell(cursor, cmd.getArgsString(), m_CellLengthArgs, m_CellFormatArgs);
  if (!cmd.comment.empty()) {
    printCell(cursor, std::string(";")+cmd.comment, m_CellLengthCmdComment, m_CellFormatCmdComment);
  }
///@bug В профайлере - это самая дорогая операция... Стоит пересмотеть способ вывода
  //cursor.movePosition(QTextCursor::End);
}

void CDisassemblerWidget::printChunkCode(QTextCursor &cursor, std::shared_ptr<GUIChunk> chunk) {
  if (!chunk->core()->label().empty()) {
    cursor.insertBlock();
    if (!chunk->core()->comment().empty()) {
      printCell(cursor, chunk->core()->addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
      printCell(cursor, std::string(), m_CellLengthOpcodes, m_CellFormatOpcodes);
      printCell(cursor, std::string(";")+chunk->core()->comment(), m_CellLengthLabel, m_CellFormatChunkComment);
      cursor.movePosition(QTextCursor::End);
    }
    printCell(cursor, chunk->core()->addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
    printCell(cursor, std::string(), m_CellLengthOpcodes, m_CellFormatOpcodes);
    printCell(cursor, chunk->core()->label()+":", m_CellLengthLabel, m_CellFormatLabel);
    printReferences(cursor, chunk);
    cursor.movePosition(QTextCursor::End);
  }
  foreach (CCommand cmd, chunk->core()->commands()) {
    cursor.insertBlock();
    printCell(cursor, cmd.addr.toString(), m_CellLengthAddr, m_CellFormatAddr);
    printCell(cursor, cmd.getOpcodesString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
    printCell(cursor, std::string(), m_CellLengthLabel, m_CellFormatLabel);
    printCell(cursor, cmd.command, m_CellLengthCommand, m_CellFormatCommand);
    printCell(cursor, cmd.getArgsString(), m_CellLengthArgs, m_CellFormatArgs);
    if (!cmd.comment.empty()) {
      printCell(cursor, std::string(";")+cmd.comment, m_CellLengthCmdComment, m_CellFormatCmdComment);
    }
    cursor.movePosition(QTextCursor::End);
  }
  cursor.movePosition(QTextCursor::End);
}

void CDisassemblerWidget::refreshView() {
  _chunks.update(m_DisassemblerCore->chunks());
  clear();
  QTextCursor cursor(textCursor());
  cursor.beginEditBlock();
  int i=0;
  for (auto chunk: _chunks.chunks()) {
    chunk->setCursorStartPosition(cursor.position()+1);
    switch (chunk->core()->type()) {
    case CChunk::Type::UNPARSED:
      printChunkUnparsed(cursor, chunk);
      break;
    case CChunk::Type::CODE:
      printChunkCode(cursor, chunk);
      break;
    };
    chunk->setCursorEndPosition(cursor.position());
    i++;
  }
  cursor.endEditBlock();
//    m_Chunks.printDebug();
  m_MainWindow->labelsWidget()->refresh();
}
