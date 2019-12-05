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

  void addView(QListWidget* wgt) {
    _wgt = wgt;
  }
  bool hasAddr() { return !_stack.empty(); }

  void push(const memory::Addr& addr) {
    _stack.push(addr);
    if (_wgt != nullptr) {
      auto lbl = core::DisassemblerCore::inst().labels().find(addr);
      if (lbl != core::DisassemblerCore::inst().labels().end()) {
        _wgt->addItem(QString::fromStdString(addr.toString() + " " + lbl->second->name));
      } else {
        _wgt->addItem(QString::fromStdString(addr.toString()));
      }
    }
  }

  memory::Addr pop() {
    memory::Addr res = _stack.top();
    _stack.pop();
    if (_wgt != nullptr) {
      auto it = _wgt->takeItem(_wgt->count() - 1);
      delete it;
    }
    return res;
  }

private:
  NavigationStack() = default;
  std::stack<memory::Addr> _stack;
  QListWidget* _wgt;
};
}
}
