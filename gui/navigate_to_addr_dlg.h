#ifndef HELPER_WIDGETS
#define HELPER_WIDGETS

#include "ui_navigate_to_addr.h"
#include "disassembler_widget.h"

class NavigateToAddrDlg : public QDialog {
  Ui::GotoAddressDlg _ui;
  DisassemblerWidget *_wdg;
Q_OBJECT;

public:
  NavigateToAddrDlg(DisassemblerWidget *wdg) : _wdg{wdg} {}

  virtual ~NavigateToAddrDlg() {}

  void operator()() {
    _ui.setupUi(this);
    _ui.addr->setFocus();
    exec();
  }

public slots:

  void accept() override {
    try {
      auto from = _wdg->getCurrentAddr();
      _wdg->navigateToAddress(from, dasm::memory::Addr(_ui.addr->text().toInt(0, 16)));
    } catch (...) {
      _wdg->navigateToAddress(dasm::memory::Addr(_ui.addr->text().toInt(0, 16)));
    }
    close();
  }

};


#endif // HELPER_WIDGETS

