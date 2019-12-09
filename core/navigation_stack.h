#pragma once

#include <stack>

namespace dasm {
namespace core {

class NavigationStack {
public:

  class Listener {
  public:
    virtual void onPush(uint16_t addr) = 0;
    virtual void onPop() = 0;
  };

  static NavigationStack& inst() {
    static NavigationStack _inst;
    return _inst;
  }

  void setListener(Listener* listener) { _listener = listener; }

  bool hasAddr() { return !_stack.empty(); }

  void push(uint16_t addr) {
    _stack.push(addr);
    if (_listener != nullptr) {
      _listener->onPush(addr);
    }
  }

  uint16_t pop() {
    uint16_t res = _stack.top();
    _stack.pop();
    if (_listener != nullptr) {
      _listener->onPop();
    }
    return res;
  }

private:
  Listener* _listener;
  NavigationStack() = default;
  std::stack<uint16_t> _stack;
};
}
}
