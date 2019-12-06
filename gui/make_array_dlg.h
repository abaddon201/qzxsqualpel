#pragma once

#include "ui_make_array.h"
#include "disassembler_widget.h"

namespace dasm {
namespace gui {

class MakeArrayDlg : public QDialog {
  Ui::MakeArrayDlg _ui;
  DisassemblerWidget* _wdg;
  Q_OBJECT;

public:
  MakeArrayDlg(DisassemblerWidget* wdg) : _wdg{ wdg } {}

  virtual ~MakeArrayDlg() {}

  void operator()() {
    _ui.setupUi(this);
    _ui.size->setFocus();
    exec();
  }

public slots:

  void accept() override {
    _wdg->makeArray(_ui.size->text().toInt(nullptr), _ui.clearMem->checkState() == Qt::Checked);
    close();
  }

};

}
}
