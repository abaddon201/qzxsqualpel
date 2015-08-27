#include <QDebug>

#include "CDisassembler.h"

#include <QFile>
#include <QTextTable>
#include <QKeyEvent>
#include <QTextDocumentWriter>
#include <QPainter>
#include "CMainWindow.h"
#include "CWidgetChangeLabel.h"


CDisassembler* CDisassembler::m_Inst = 0;
IDisassemblerCore* CDisassembler::m_DisassemblerCore = 0;

CDisassembler::CDisassembler()
  : QPlainTextEdit() {
  init();
}

CDisassembler::CDisassembler(CMainWindow* mwnd)
  : QPlainTextEdit()
  , m_MainWindow(mwnd) {

  init();
}

void CDisassembler::init() {
  QFont font;
  font.setFamily("Courier");
  font.setFixedPitch(true);
  font.setPointSize(10);
  setFont(font);
  m_Inst=this;

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

void CDisassembler::navigateToLabel(int num) {
  if (m_DisassemblerCore->labels().count()>num) {
    const CLabel &label=m_DisassemblerCore->labels().at(num);
    qDebug()<< label;
    const CChunk* chunk = m_DisassemblerCore->chunks().getChunkContains(label.addr());
    QTextCursor cursor(textCursor());
    cursor.setPosition(chunk->cursorStartPosition());
    setTextCursor(cursor);
  }
}

void CDisassembler::paintEvent(QPaintEvent* event) {
  QPlainTextEdit::paintEvent(event);
  QPainter p(viewport());
  QRect cr = cursorRect();
  cr.setRight(cr.right()+5);
  p.fillRect(cr, QBrush(QColor(0, 100, 255, 120)));
}

void CDisassembler::keyPressEvent(QKeyEvent* event) {
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

void CDisassembler::changeNameUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  CChunk* chunk=m_DisassemblerCore->chunks().getChunkByPosition(cursor.position());
  if (0==chunk) {
    return;
  }
  if (chunk) {
    if ((chunk->type()==CChunk::Type::CODE)
        || (chunk->type()==CChunk::Type::DATA_BYTE)
        || (chunk->type()==CChunk::Type::DATA_ARRAY)
        || (chunk->type()==CChunk::Type::DATA_WORD)
       ) {
      CWidgetChangeLabel dlg(this, chunk->label());
      if (dlg.exec()) {
        m_DisassemblerCore->labels().changeLabel(chunk, dlg.label());
        refreshView();
      }
    }
  }
}

void CDisassembler::makeCodeUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  CChunk* chunk=m_DisassemblerCore->chunks().getChunkByPosition(cursor.position());
  if (0==chunk) {
    return;
  }
  if (chunk && chunk->isEmpty()) {
    m_DisassemblerCore->disassembleBlock(chunk->addr());
    refreshView();
  }
}

void CDisassembler::openRAWFile(QString fileName) {
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

void CDisassembler::saveASMFile(QString fileName) {
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

void CDisassembler::printCell(QTextCursor &cursor, QString text, int length, QTextCharFormat fmt) {
  int spclen=length-text.length();
  if (spclen<0) {
    spclen=0;
  }
  QString spcline(spclen, ' ');
  cursor.insertText(text+spcline, fmt);
}

void CDisassembler::printCell(QTextCursor &cursor, QString text, int length) {
  int spclen=length-text.length();
  if (spclen<0) {
    spclen=0;
  }
  QString spcline(spclen, ' ');
  cursor.insertText(text+spcline);
}

void CDisassembler::printReferences(QTextCursor &cursor, CChunk* chunk) {
  if (chunk->references.count()==0) {
    return;
  }
  int skip_len=m_CellLengthCommand+m_CellLengthArgs;
  int skip_len2=m_CellLengthAddr+m_CellLengthOpcodes+m_CellLengthLabel+m_CellLengthCommand+m_CellLengthArgs;

  QList<CReference>::const_iterator it;
  int l=0;
  for (it=chunk->references.begin(); it!=chunk->references.end(); ++it) {
    if (l==0) {
      printCell(cursor, QString(), skip_len);
      l=1;
    } else {
      printCell(cursor, QString(), skip_len2);
    }
    for (int i=0; i<m_ReferencesOnLine; i++) {
      CReference ref=*it;
      printCell(cursor, ref.toString(), m_CellLengthReference, m_CellFormatReference);
      ++it;
      if (it==chunk->references.end()) {
        return;
      }
    }
    --it;
    cursor.insertText("\n");
  }
}

void CDisassembler::printChunkUnparsed(QTextCursor &cursor, CChunk* chunk) {
  cursor.insertBlock();
  CCommand cmd=chunk->getCommand(0);
  printCell(cursor, cmd.addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
  printCell(cursor, cmd.getOpcodesString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
  printCell(cursor, QString(), m_CellLengthLabel, m_CellFormatLabel);
  printCell(cursor, cmd.command, m_CellLengthCommand, m_CellFormatCommand);
  printCell(cursor, cmd.getArgsString(), m_CellLengthArgs, m_CellFormatArgs);
  if (!cmd.comment().isEmpty()) {
    printCell(cursor, QString(";")+cmd.comment(), m_CellLengthCmdComment, m_CellFormatCmdComment);
  }

  cursor.movePosition(QTextCursor::End);
}

void CDisassembler::printChunkCode(QTextCursor &cursor, CChunk* chunk) {
  if (!chunk->label().isEmpty()) {
    cursor.insertBlock();
    if (!chunk->comment.isEmpty()) {
      printCell(cursor, chunk->addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
      printCell(cursor, QString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
      printCell(cursor, QString(";")+chunk->comment, m_CellLengthLabel, m_CellFormatChunkComment);
      cursor.movePosition(QTextCursor::End);
    }
    printCell(cursor, chunk->addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
    printCell(cursor, QString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
    printCell(cursor, chunk->label()+":", m_CellLengthLabel, m_CellFormatLabel);
    printReferences(cursor, chunk);
    cursor.movePosition(QTextCursor::End);
  }
  foreach (CCommand cmd, chunk->commands()) {
    cursor.insertBlock();
    printCell(cursor, cmd.addr().toString(), m_CellLengthAddr, m_CellFormatAddr);
    printCell(cursor, cmd.getOpcodesString(), m_CellLengthOpcodes, m_CellFormatOpcodes);
    printCell(cursor, QString(), m_CellLengthLabel, m_CellFormatLabel);
    printCell(cursor, cmd.command, m_CellLengthCommand, m_CellFormatCommand);
    printCell(cursor, cmd.getArgsString(), m_CellLengthArgs, m_CellFormatArgs);
    if (!cmd.comment().isEmpty()) {
      printCell(cursor, QString(";")+cmd.comment(), m_CellLengthCmdComment, m_CellFormatCmdComment);
    }
    cursor.movePosition(QTextCursor::End);
  }
  cursor.movePosition(QTextCursor::End);
}

void CDisassembler::refreshView() {
  clear();
  QTextCursor cursor(textCursor());
  cursor.beginEditBlock();
  CChunkList::iterator it=m_DisassemblerCore->chunks().begin();
  for (; it!=m_DisassemblerCore->chunks().end(); ++it) {
    CChunk* chunk=*it;
    chunk->setCursorStartPosition(cursor.position()+1);
    switch (chunk->type()) {
    case CChunk::Type::UNPARSED:
      printChunkUnparsed(cursor, chunk);
      break;
    case CChunk::Type::CODE:
      printChunkCode(cursor, chunk);
      break;
    };
    chunk->setCursorEndPosition(cursor.position());
  }
  cursor.endEditBlock();
//    m_Chunks.printDebug();
  m_MainWindow->labelsWidget()->refresh();
}
