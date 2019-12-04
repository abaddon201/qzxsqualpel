#ifndef GUICHUNK_H
#define GUICHUNK_H

template <typename T>
class GUIBlock {
public:
  void setCursorEndPosition(int pos) { _end_cursor_position = pos; }

  void setCursorStartPosition(int pos) { _start_cursor_position = pos; }

  int cursorEndPosition() const { return _end_cursor_position; }

  int cursorStartPosition() const { return _start_cursor_position; }

  void makeCopy(const GUIBlock& ch) {
    _start_cursor_position = ch._start_cursor_position;
    _end_cursor_position = ch._end_cursor_position;
  }

  std::shared_ptr<T> core() { return _parent; }

  explicit GUIBlock(std::shared_ptr<T> c) : _parent{ c } {}

private:
  std::shared_ptr<T> _parent;
  ///@brief Позиция курсора в редакторе
  int _start_cursor_position;
  int _end_cursor_position;
};

#endif // GUICHUNK_H
