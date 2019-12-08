#pragma once

#include <vector>

namespace dasm {
namespace core {

template<typename T>
class MemoryMap {
public:
  class Element {
  public:
    explicit Element(T& elem) { _elem = elem; _offs = 0; }
    explicit Element(size_t offs) { _offs = offs; }
    Element() { _offs = -1; }
    //FIXME: check if _offs != 0
    T& elem() { return _elem; }
    const T& elem() const { return _elem; }
    void setElem(T& elem) { _elem = elem; _offs = 0; }
    int offset() const { return _offs; }
    void setOffset(size_t offs) { _offs = offs; }
  private:
    T _elem;
    size_t _offs;
  };

  MemoryMap() {}

  void clear() { _elems.clear(); }
  void reset(size_t size) { _elems.resize(size); }

  void put(size_t pos, size_t len, T& elem) {
    _elems[pos].setElem(elem);
    if (len > 1) {
      for (size_t i = 1; i < len; ++i) {
        _elems[pos + i].setOffset(i);
      }
    }
  }

  size_t end() const {
    return std::numeric_limits<size_t>::max();
  }

  template<class _Pr>
  size_t find_position(_Pr pred) {
    size_t pos = 0;
    for (auto& e : _elems) {
      if (e.offset() == 0 && pred(e)) {
        return pos;
      }
      ++pos;
    }
    return std::numeric_limits<size_t>::max();
  }

  T& get(size_t pos) {
    if (_elems[pos].offset() != 0) {
      return _elems[pos - _elems[pos].offset()].elem();
    }
    return _elems[pos].elem();
  }

  bool get_if(size_t pos, T& res) {
    if (_elems[pos].offset() != 0) {
      return false;
    }
    res = _elems[pos].elem();
    return true;
  }

  const std::vector<Element>& whole() const { return _elems; }
private:
  std::vector<Element> _elems;
};
}
}
