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
    explicit Element(int offs) { _offs = offs; }
    Element() { _offs = -1; }
    //FIXME: check if _offs ==-1
    T& elem() { return _elem; }
    const T& elem() const { return _elem; }
    int offset() const { return _offs; }
  private:
    T _elem;
    int _offs;
  };

  MemoryMap() {}

  void clear() { _elems.clear(); }
  void reset(size_t size) { _elems.resize(size); }

  void put(int pos, int len, T& elem) {
    _elems[pos] = Element(elem);
    if (len > 1) {
      for (int i = 1; i <= len; ++i) {
        _elems[pos + i] = Element(i);
      }
    }
  }

  T& get(int pos) {
    if (_elems[pos].offset() != 0) {
      return _elems[pos - _elems[pos].offset()].elem();
    }
    return _elems[pos].elem();
  }
  const std::vector<Element>& whole() const { return _elems; }
private:
  std::vector<Element> _elems;
};
}
}
