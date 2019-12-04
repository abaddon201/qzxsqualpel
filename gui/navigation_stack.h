#pragma once

#include <stack>

namespace dasm {
namespace gui {

class NavigationStack {
public:
  static NavigationStack& inst() {
    static NavigationStack _inst;
    return _inst;
  }

  bool hasAddr() { return !_stack.empty(); }

  void push(const memory::Addr& addr) {
    _stack.push(addr);
  }

  memory::Addr pop() {
    memory::Addr res = _stack.top();
    _stack.pop();
    return res;
  }

private:
  NavigationStack() = default;
  std::stack<memory::Addr> _stack;
};
}
}
