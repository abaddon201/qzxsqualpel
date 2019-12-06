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

  void push(uint16_t addr) {
    _stack.push(addr);
    if (_wgt != nullptr) {
      auto lbl = core::DisassemblerCore::inst().labels().find(addr);
      if (lbl != core::DisassemblerCore::inst().labels().end()) {
        _wgt->addItem(QString::fromStdString(utils::toHex(addr) + " " + lbl->second->name));
      } else {
        _wgt->addItem(QString::fromStdString(utils::toHex(addr)));
      }
    }
  }

  uint16_t pop() {
    uint16_t res = _stack.top();
    _stack.pop();
    if (_wgt != nullptr) {
      auto it = _wgt->takeItem(_wgt->count() - 1);
      delete it;
    }
    return res;
  }

private:
  NavigationStack() = default;
  std::stack<uint16_t> _stack;
  QListWidget* _wgt;
};
}
}
