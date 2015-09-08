#ifndef GUICHUNK_H
#define GUICHUNK_H

#include "core/CChunk.h"

class GUIChunk {
public:
  void setCursorEndPosition(int pos) {_end_cursor_position=pos;}
  void setCursorStartPosition(int pos) {_start_cursor_position=pos;}
  int cursorEndPosition() const {return _end_cursor_position;}
  int cursorStartPosition() const {return _start_cursor_position;}

  void makeCopy(const GUIChunk &ch) {
    _start_cursor_position=ch._start_cursor_position;
    _end_cursor_position=ch._end_cursor_position;
  }

  std::shared_ptr<Chunk> core() {return _parent;}
  GUIChunk(std::shared_ptr<Chunk> c) : _parent{c} {
    if (!c) {
      int i=4;
      i--;
    }
  }

private:
  std::shared_ptr<Chunk> _parent;
  ///@brief Позиция курсора в редакторе
  int _start_cursor_position;
  int _end_cursor_position;
};

#endif // GUICHUNK_H

