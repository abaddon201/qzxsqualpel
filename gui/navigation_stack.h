#pragma once

#include <stack>

#include "core/navigation_stack.h"

namespace dasm {
namespace gui {

class NavigationStack : public core::NavigationStack::Listener {
public:
  static NavigationStack& inst() {
    static NavigationStack _inst;
    return _inst;
  }

  void addView(QListWidget* wgt) {
    _wgt = wgt;
  }
  void onPush(uint16_t addr) override {
    if (_wgt != nullptr) {
      auto lbl = core::DisassemblerCore::inst().labels().find(addr);
      if (lbl != core::DisassemblerCore::inst().labels().end()) {
        _wgt->addItem(QString::fromStdString(utils::toHex(addr) + " " + lbl->second->name));
      } else {
        _wgt->addItem(QString::fromStdString(utils::toHex(addr)));
      }
    }
  }

  void onPop() {
    if (_wgt != nullptr) {
      auto it = _wgt->takeItem(_wgt->count() - 1);
      delete it;
    }
  }

private:
  NavigationStack() = default;
  QListWidget* _wgt = nullptr;
};
}
}
