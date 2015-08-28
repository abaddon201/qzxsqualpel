//
// C++ Implementation: CDisassemblerCoreZX
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QDebug>

#include "disassemble.h"

#include "CDisassemblerCoreZX.h"

//compatibility hack for fuse disassembler
unsigned char readbyte_internal(unsigned short addr) {
  return IDisassemblerCore::inst()->getMemoryByte(addr);
}

IDisassemblerCore::Type CDisassemblerCoreZX::getLastCmdJumpType(CChunk* chunk, CAddr &jump_addr) {
  CCommand cmd=chunk->lastCommand();
  if (cmd.command.toLower()=="call") {
    jump_addr=cmd.getJmpAddr();
    return Type::JT_CALL;
  }
  if (((cmd.command.toLower()=="jr") || (cmd.command.toLower()=="jp")) && (cmd.arg2.isEmpty())) {
    if (cmd.arg1.startsWith("(") || cmd.arg1.toLower().contains("ix") || cmd.arg1.toLower().contains("iy") || cmd.arg1.toLower().contains("hl")) {
      //jump to (HL) or (IX) or (IY)
      return Type::JT_RET;
    }
    jump_addr=cmd.getJmpAddr();
    return Type::JT_JUMP;
  }
  if (((cmd.command.toLower()=="jr") || (cmd.command.toLower()=="jp")) && (!cmd.arg2.isEmpty())) {
    jump_addr=cmd.getJmpAddr();
    return Type::JT_COND_JUMP;
  }
  if ((cmd.command.toLower()=="ret") && (!cmd.arg1.isEmpty())) {
    return Type::JT_COND_RET;
  }
  if ((cmd.command.toLower()=="ret") && (cmd.arg1.isEmpty())) {
    return Type::JT_RET;
  }
  if (cmd.command.toLower()=="reti") {
    return Type::JT_RET;
  }
  if (cmd.command.toLower()=="retn") {
    return Type::JT_RET;
  }
  return Type::JT_NONE;
}

int CDisassemblerCoreZX::disassembleInstruction(CAddr addr) {
  char tbuff[128];
  size_t len;
  debugger_disassemble( tbuff, 128, &len, addr.offset() );

  if (len) {
    QString buff{tbuff};
    //initialize chunk
    CChunk* chunk_i=0;
    chunk_i=m_Chunks.getChunk(addr);
    if (chunk_i==0) {
      qDebug()<<"no instruction here";
      return 0;
    }
    if (!chunk_i->isEmpty())  {
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
    qDebug()<<"addr="<<addr.toString()<<"command=" << buff <<"len=" << len;
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
    cmd.addr = addr;
    QStringList strlist=buff.split(" ");
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
      foreach (const CChunk &cc, chunks) {
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

void CDisassemblerCoreZX::disassembleBlock(CAddr addr) {
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
    switch (getLastCmdJumpType(chunk, jump_addr)) {
    case IDisassemblerCore::Type::JT_CALL:
      //call
      qDebug()<<"call: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      qDebug()<<"st_addr="<<st_addr.toString();
      makeJump(addr, jump_addr, CReference::Type::CALL);
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_COND_JUMP:
      //conditional jump
      qDebug()<<"cond jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      makeJump(addr, jump_addr, CReference::Type::JUMP);
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_JUMP:
      qDebug()<<"jump: addr=" <<addr.toString()<< "to_addr" <<jump_addr.toString();
      makeJump(addr, jump_addr, CReference::Type::JUMP);
      res=0;
      break;
    case IDisassemblerCore::Type::JT_COND_RET:
      //conditional ret
      qDebug()<<"cond_ret: addr=" <<addr.toString();
      addr+=res;
      break;
    case IDisassemblerCore::Type::JT_RET:
      qDebug()<<"ret: addr=" <<addr.toString();
      res=0;
      break;
    case IDisassemblerCore::Type::JT_NONE:
      addr+=res;
      break;
    }
  } while (res);
  qDebug()<<"finished chunk:st_addr="<<st_addr.toString()/*<<"commands count="<<chunk->commandsCount()*/<<"m_chunks.count="<<m_Chunks.count();
}

bool CDisassemblerCoreZX::labelPresent(CAddr addr) const {
  if (m_Labels.count()) {
    foreach (CLabel lbl, m_Labels) {
      if (lbl.addr==addr) {
        return true;
      }
    }
  }
  return false;
}

CByte CDisassemblerCoreZX::getMemoryByte(CAddr addr) const {
  return m_MemoryPool[addr.offset()&0xFFFF];
}

CChunk* CDisassemblerCoreZX::createChunk(CAddr addr, CChunk::Type type) {
  return m_Chunks.createChunk(addr, type);
}

void CDisassemblerCoreZX::makeJump(CAddr from_addr, CAddr jump_addr, CReference::Type ref_type) {
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

void CDisassemblerCoreZX::setRawMemory(unsigned char* buf, size_t size) {
  m_ProgLength = size;
  memcpy(m_MemoryPool, buf, size);
  ///@fixme must chek sizes
  m_ProgLength = 10000;
  ///@fixme remove limitation
}

void CDisassemblerCoreZX::initialParse() {
  m_Chunks.clear();
  for (int i=0; i<m_ProgLength; ++i) {
    CChunk* chunk=m_Chunks.createChunk(i, CChunk::Type::UNPARSED);
    CByte byte=m_MemoryPool[i];
    CCommand cmd;
    cmd.command="db";
    cmd.addr = i;
    cmd.opcodes.append(byte);
    cmd.arg1=byte.toString();
    chunk->appendCommand(cmd);
  }
  updater->updateWidgets();
}

