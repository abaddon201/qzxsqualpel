#pragma once

#include <vector>

namespace dasm {
namespace core {

class NavigationStack {
public:

  class Listener {
  public:
    virtual void onPush(uint16_t addr) = 0;
    virtual void onPop() = 0;
  };

  using StackItem = uint16_t;

  static NavigationStack& inst() {
    static NavigationStack _inst;
    return _inst;
  }

  void setListener(Listener* listener) { _listener = listener; }

  bool hasAddr() { return !_stack.empty(); }

  void clear() { _stack.clear(); }

  void push(uint16_t addr) {
    _stack.push_back(addr);
    if (_listener != nullptr) {
      _listener->onPush(addr);
    }
  }

  uint16_t pop() {
    uint16_t res = _stack.back();
    _stack.pop_back();
    if (_listener != nullptr) {
      _listener->onPop();
    }
    return res;
  }

  const std::vector<StackItem>& items() const { return _stack; }

private:
  Listener* _listener;
  NavigationStack() = default;
  std::vector<StackItem> _stack;
};
}
}
