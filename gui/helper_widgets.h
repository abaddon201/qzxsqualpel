#ifndef HELPER_WIDGETS
#define HELPER_WIDGETS

#include "ui_goto_address.h"
#include "CDisassemblerWidget.h"

class NavigateToAddrDlg : public QDialog {
  //QDialog* dlg = new QDialog;
  Ui::GotoAddressDlg ui;
  CDisassemblerWidget* _wdg;
  Q_OBJECT;
public:

  NavigateToAddrDlg(CDisassemblerWidget* wdg):_wdg{wdg} {}
  virtual ~NavigateToAddrDlg() {}
  void operator() () {
    ui.setupUi(this);
    ui.addr->setFocus();
    exec();
  }

public slots:
  void accept() {
    _wdg->navigateToAddress(CAddr(ui.addr->text().toInt(0, 16)));
    close();
  }

};


#endif // HELPER_WIDGETS

