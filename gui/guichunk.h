#ifndef GUICHUNK_H
#define GUICHUNK_H

#include "core/chunk.h"

class GUIChunk {
public:
  void setCursorEndPosition(int pos) { _end_cursor_position = pos; }

  void setCursorStartPosition(int pos) { _start_cursor_position = pos; }

  int cursorEndPosition() const { return _end_cursor_position; }

  int cursorStartPosition() const { return _start_cursor_position; }

  void makeCopy(const GUIChunk &ch) {
    _start_cursor_position = ch._start_cursor_position;
    _end_cursor_position = ch._end_cursor_position;
  }

  std::shared_ptr<dasm::core::Chunk> core() { return _parent; }

  GUIChunk(std::shared_ptr<dasm::core::Chunk> c) : _parent{c} {
  }

private:
  std::shared_ptr<dasm::core::Chunk> _parent;
  ///@brief Позиция курсора в редакторе
  int _start_cursor_position;
  int _end_cursor_position;
};

using GUIChunkPtr = std::shared_ptr<GUIChunk>;

#endif // GUICHUNK_H

