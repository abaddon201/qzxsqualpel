#ifndef HELPER_WIDGETS
#define HELPER_WIDGETS

#include "ui_navigate_to_addr.h"
#include "disassembler_widget.h"
#include "document_helper.h"

namespace dasm {
namespace gui {

class NavigateToAddrDlg : public QDialog {
  Ui::GotoAddressDlg _ui;
  DisassemblerWidget* _wdg;
  Q_OBJECT;

public:
  NavigateToAddrDlg(DisassemblerWidget* wdg) : _wdg{ wdg } {}

  virtual ~NavigateToAddrDlg() {}

  void operator()() {
    _ui.setupUi(this);
    _ui.addr->setFocus();
    exec();
  }

public slots:

  void accept() override {
    try {
      auto from = DocumentHelper::inst().getAddrFromLineStart();
      _wdg->navigateToAddress(from, utils::fromHex(_ui.addr->text().toStdString()));
    } catch (...) {
      _wdg->navigateToAddress(utils::fromHex(_ui.addr->text().toStdString()));
    }
    close();
  }

};

}
}

#endif // HELPER_WIDGETS

