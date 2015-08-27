#include <QDebug>

#include "CDisassembler.h"

#include <QFile>
#include <QTextTable>
#include <QKeyEvent>
#if QT_VERSION>=0x040500
#include <QTextDocumentWriter>
#endif
#include "CMainWindow.h"
#include "CWidgetChangeLabel.h"


//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return CDisassembler::inst()->getMemoryByte(addr).toUChar();
}

CDisassembler* CDisassembler::m_Inst=0;

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

  m_CellLengthAddr=CELLLENGTHADDR;
  m_CellLengthOpcodes=CELLLENGTHOPCODES;
  m_CellLengthLabel=CELLLENGTHLABEL;
  m_CellLengthCommand=CELLLENGTHCOMMAND;
  m_CellLengthArgs=CELLLENGTHARGS;
  m_CellLengthCmdComment=CELLLENGTHCMDCOMMENT;
  m_CellLengthReference=CELLLENGTHREFERENCE;

  m_ReferencesOnLine=3;
}

bool CDisassembler::labelPresent(CAddr addr) {
  if (m_Labels.count()) {
    foreach (CLabel lbl, m_Labels) {
      if (lbl.addr()==addr) {
        return true;
      }
    }
  }
  return false;
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

CByte CDisassembler::getMemoryByte(CAddr addr) {
  return m_MemoryPool[addr.offset()&0xFFFF];
}

CChunk* CDisassembler::createChunk(CAddr addr, CChunk::Type type) {
  return m_Chunks.createChunk(addr, type);
}

void CDisassembler::changeNameUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  CChunk* chunk=m_Chunks.getChunkByPosition(cursor.position());
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
        m_Labels.changeLabel(chunk, dlg.label());
        refreshView();
      }
    }
  }
}

void CDisassembler::makeCodeUnderCursor() {
  QTextCursor cursor(textCursor());
  qDebug()<<"!!!!!!!!!!!:"<<cursor.block().text();
  qDebug()<<"Cursor pos:"<<cursor.position();
  CChunk* chunk=m_Chunks.getChunkByPosition(cursor.position());
  if (0==chunk) {
    return;
  }
  if (chunk && isChunkEmpty(chunk)) {
    disassembleBlock(chunk->addr());
    refreshView();
  }
}

bool CDisassembler::isChunkEmpty(CChunk* chunk) {
  if ((chunk->type()!=CChunk::Type::UNPARSED) && (chunk->type()!=CChunk::Type::UNKNOWN)) {
    return false;
  }
  return true;
}

int CDisassembler::disassembleInstruction(CAddr addr) {
  QString res_str;
  size_t len=m_DisassemblerCore->disassembleInstruction(&res_str, addr);
  if (len) {
    //initialize chunk
    CChunk* chunk_i=0;
    chunk_i=m_Chunks.getChunk(addr);
    if (chunk_i==0) {
      qDebug()<<"no instruction here";
      return 0;
    }
    if (!isChunkEmpty(chunk_i))  {
      qDebug()<<"allready parsed";
      return 0;
    }
    CByte opcode=chunk_i->getCommand(0).opcodes[0];
    m_Chunks.removeChunk(addr);
    CChunk* target_chunk;
    if (addr==0) {
      //parsing current addr
      qDebug()<<"disassembleInstruction, zero addr";
      target_chunk=m_Chunks.createChunk(addr, CChunk::Type::CODE);
    } else {
      qDebug()<<"disassembleInstruction, nonzero addr";
      target_chunk=m_Chunks.getChunkContains(addr-1);
      if (target_chunk==0) {
        qDebug()<<"No target for disassemble";
        target_chunk=m_Chunks.createChunk(addr, CChunk::Type::CODE);
      }
      //appending to prev. parsed block
      qDebug()<<"target_chunk type:"<<target_chunk->type()<<"addr:"<<target_chunk->addr().toString();
      if (target_chunk->type()!=CChunk::Type::CODE) {
        qDebug()<<"Not code previous chunk";
        //parsing current addr
        target_chunk=m_Chunks.createChunk(addr, CChunk::Type::CODE);
      }
    }
    qDebug()<<"target_chunk2 type:"<<target_chunk->type()<<"addr:"<<target_chunk->addr().toString();
    qDebug()<<"addr="<<addr.toString()<<"command=" << res_str <<"len=" << len;
    ///@bug must be in segment range check... think about it
    if (addr+len>=m_Chunks.getMaxAddr()) {
      qDebug()<<"instruction out of mem block";
      return -3;
    }
    QList<CChunk> chunks;
    if (len>1) {
      for (size_t i=1; i<len; i++) {
        CChunk* ch=m_Chunks[addr+i];
        if (ch==0) {
          qDebug()<<"Instrunction longer than unparsed block";
          return -4;
        }
        if (ch->type()!=CChunk::Type::UNPARSED) {
          qDebug()<<"Instrunction longer than unparsed block2";
          return -4;
        }
      }
      for (size_t i=1; i<len; i++) {
        chunks.append(*m_Chunks.getChunk(addr+i));
        m_Chunks.removeChunk(addr+i);
      }
    }
    CCommand cmd;
    cmd.setAddr(addr);
    QStringList strlist=res_str.split(" ");
    cmd.command=strlist[0];
    qDebug()<<"strlist1="<<strlist;
    if (strlist.count()>1) {
      //has args
      QStringList args=strlist[1].split(",");
      qDebug()<<"argsstrlist1="<<args;
      cmd.arg1=args[0];
      if (args.count()==2) {
        cmd.arg2=args[1];
        qDebug()<<"arg2="<<cmd.arg2;
      }
    }
    cmd.opcodes.append(opcode);
    qDebug()<<"opcode appended";
    if (chunks.count()) {
      foreach (CChunk cc, chunks) {
        //becourse we works only from undefined chunks, we able to do this like that
        cmd.opcodes.append(cc.getCommand(0).opcodes[0]);
      }
    }
    qDebug()<<"all opcodes appended";
    target_chunk->appendCommand(cmd);
    qDebug()<<"cmd appended";
  }
  return len;
}

void CDisassembler::makeJump(CAddr from_addr, CAddr jump_addr, CReference::ReferenceType ref_type) {
  disassembleBlock(jump_addr);
  CChunk* jmp_chunk=m_Chunks.getChunk(jump_addr);
  if (jmp_chunk) {
    jmp_chunk->addCrossRef(from_addr, ref_type);
    QString lbl;
    if (jmp_chunk->label().isEmpty()) {
      lbl=jmp_chunk->setLabel(QString(), ref_type);
    } else {
      lbl=jmp_chunk->label();
    }
    if (!labelPresent(jump_addr)) {
      CLabel label(jump_addr, lbl);
      m_Labels.append(label);
    }
  } else {
    qDebug()<<"Split chunk at jump";
    // split target chunk
    CChunk* near_chunk=m_Chunks.getChunkContains(jump_addr);
    if (near_chunk==0) {
      qDebug()<<"Fatal error on split: No target chunk";
      return;
    }
    qDebug()<<"near_chunk:addr"<<near_chunk->addr().toString();
    CChunk* jmp_chunk=near_chunk->splitAt(jump_addr);
    if (jmp_chunk==0) {
      qDebug()<<"Split impossible";
      return;
    }
    qDebug()<<"jmp_chunk:addr"<<jmp_chunk->addr().toString();
    jmp_chunk->addCrossRef(from_addr, ref_type);
    QString lbl;
    if (jmp_chunk->label().isEmpty()) {
      lbl=jmp_chunk->setLabel(QString(), ref_type);
    } else {
      lbl=jmp_chunk->label();
    }
    if (!labelPresent(jump_addr)) {
      CLabel label(jump_addr, lbl);
      m_Labels.append(label);
    }
  }
}

void CDisassembler::disassembleBlock(CAddr addr) {
  int res=0;
  CAddr st_addr=addr;
  qDebug()<<"disassembleBlock(): addr"<< addr.toString();
  do {
    res=disassembleInstruction(addr);
    if (res==0) {
      //end of block
      break;
    }
    if (res<0) {
      //parse error
      return;
    }
    CAddr jump_addr;
    CChunk* chunk=m_Chunks.getChunkContains(addr);
    if (chunk==0) {
      qDebug()<<"No chunk after disassemble instruction, addr:"<<addr.toString();
      return;
    }
    switch (m_DisassemblerCore->getLastCmdJumpType(chunk, jump_addr)) {
    case IDisassemblerCore::JT_CALL:
      //call
      qDebug()<<"call: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      qDebug()<<"st_addr="<<st_addr.toString();
      makeJump(addr, jump_addr, CReference::CALL);
      addr+=res;
      break;
    case IDisassemblerCore::JT_COND_JUMP:
      //conditional jump
      qDebug()<<"cond jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      makeJump(addr, jump_addr, CReference::JUMP);
      addr+=res;
      break;
    case IDisassemblerCore::JT_JUMP:
      qDebug()<<"jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      makeJump(addr, jump_addr, CReference::JUMP);
      res=0;
      break;
    case IDisassemblerCore::JT_COND_RET:
      //conditional ret
      qDebug()<<"cond_ret: addr=" <<addr.toString();
      addr+=res;
      break;
    case IDisassemblerCore::JT_RET:
      qDebug()<<"ret: addr=" <<addr.toString();
      res=0;
      break;
    case IDisassemblerCore::JT_NONE:
      addr+=res;
      break;
    }
  } while (res);
  qDebug()<<"finished chunk:st_addr="<<st_addr.toString()/*<<"commands count="<<chunk->commandsCount()*/<<"m_chunks.count="<<m_Chunks.count();
}

void CDisassembler::openRAWFile(QString fileName) {
  FILE* f=fopen(fileName.toLocal8Bit(), "rb");
  m_ProgLength=fread(m_MemoryPool, 1, 65536, f);
  fclose(f);
  initialParse();
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

void CDisassembler::initialParse() {
  m_Chunks.clear();
  m_ProgLength=10000;
  for (int i=0; i<m_ProgLength; ++i) {
    CChunk* chunk=m_Chunks.createChunk(i, CChunk::Type::UNPARSED);
    CByte byte=m_MemoryPool[i];
    CCommand cmd;
    cmd.command="db";
    cmd.setAddr(i);
    cmd.opcodes.append(byte);
    cmd.arg1=byte.toString();
    chunk->appendCommand(cmd);
  }
  refreshView();
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
  CChunkList::iterator it=m_Chunks.begin();
  for (; it!=m_Chunks.end(); ++it) {
    CChunk* chunk=*it;
    chunk->setCursorStartPosition(cursor.position());
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
