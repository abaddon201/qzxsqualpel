#pragma once

class GUIBlock {
public:
  void setCursorEndPosition(int pos) { _end_cursor_position = pos; }

  void setCursorStartPosition(int pos) { _start_cursor_position = pos; }

  int cursorEndPosition() const { return _end_cursor_position; }

  int cursorStartPosition() const { return _start_cursor_position; }

  void shiftPosition(int offset) {
    _start_cursor_position += offset;
    _end_cursor_position += offset;
  }

private:
  int _start_cursor_position;
  int _end_cursor_position;
};

using GUIBlockPtr = std::shared_ptr<GUIBlock>;
