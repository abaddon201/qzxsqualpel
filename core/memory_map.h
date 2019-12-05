#pragma once

#include <vector>

namespace dasm {
namespace core {

template<typename T>
class MemoryMap {
public:
  class Element {
  public:
    Element(T& elem) { _elem = elem; _offs = 0; }
    Element(int offs) { _offs = offs; }

    T& elem() { return _elem; }
    int offset() { return _offs; }
  private:
    T _elem;
    int offs;
  };

  MemoryMap() {}

  void clear() { _elems.clear(); }
  void reset(size_t size) { _elems.resize(size); }

  void put(int pos, int len, T& elem) {
    _elems[pos] = Element(elem);
    if (len > 1) {
      for (int i = 1; i <= len; ++i) {
        _elem[pos + i] = Element(i);
      }
    }
  }

  T& get(int pos) {
    if (_elems[pos].offset() != 0) {
      return _elems[pos - _elems[pos].offset()].elem();
    }
    return _elems[pos].elem();
  }
private:
  std::vector<Element> _elems;
};
}
}
