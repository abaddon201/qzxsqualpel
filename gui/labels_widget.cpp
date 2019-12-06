//
// C++ Implementation: CLabelsWidget
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "labels_widget.h"

#include "core/label.h"
#include "disassembler_widget.h"

namespace dasm {
namespace gui {

LabelsWidget::LabelsWidget(QWidget* par, DisassemblerWidget* disasm)
  : QTableWidget(par),
  _disasm{ disasm } {
  setSortingEnabled(true);
  setMinimumWidth(400);
  connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(jumpToLabel(int, int)));
}

Q_DECLARE_METATYPE(uint16_t)

void LabelsWidget::refresh() {
  clear();
  setColumnCount(2);
  QStringList strlist;
  strlist << tr("Name") << tr("Address");
  setHorizontalHeaderLabels(strlist);
  setSortingEnabled(false);
  int i = 0;
  dasm::core::Labels& my_labels = dasm::core::DisassemblerCore::inst().labels();
  setRowCount((int)my_labels.size());
  for (auto lbl : my_labels) {
    QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(lbl.second->name));
    setItem(i, 0, nameItem);
    QTableWidgetItem* addrItem = new QTableWidgetItem(QString::fromStdString(utils::toHex(lbl.second->addr)));
    setItem(i, 1, addrItem);
    QVariant var;
    var.setValue<uint16_t>(lbl.second->addr);
    nameItem->setData(Qt::UserRole, var);
    i++;
  }
  setSortingEnabled(true);
}

void LabelsWidget::jumpToLabel(int row, int) {
  uint16_t addr = item(row, 0)->data(Qt::UserRole).value<uint16_t>();
  _disasm->navigateToAddress(addr);
}

}
}
