#ifndef GUICHUNK_H
#define GUICHUNK_H

#include "core/CChunk.h"

class GUIChunk {
public:
  void setCursorEndPosition(int pos) {m_EndCursorPosition=pos;}
  void setCursorStartPosition(int pos) {m_StartCursorPosition=pos;}
  int cursorEndPosition() const {return m_EndCursorPosition;}
  int cursorStartPosition() const {return m_StartCursorPosition;}

  void makeCopy(const GUIChunk &ch) {
    m_StartCursorPosition=ch.m_StartCursorPosition;
    m_EndCursorPosition=ch.m_EndCursorPosition;
  }

  std::shared_ptr<CChunk> core() {return _parent;}
  GUIChunk(std::shared_ptr<CChunk> c) : _parent{c} { if (!c) {
      int i=4;
      i--;
    }
    }
private:
  std::shared_ptr<CChunk> _parent;
  ///@brief Позиция курсора в редакторе
  ///@bug Должны быть в другом месте
  int m_StartCursorPosition;
  int m_EndCursorPosition;
};

#endif // GUICHUNK_H

