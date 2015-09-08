//
// C++ Implementation: CLabelsWidget
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "labels_widget.h"

#include "core/label.h"
#include "disassembler_widget.h"

LabelsWidget::LabelsWidget(QWidget* par, DisassemblerWidget* disasm)
  : QTableWidget(par),
    _disasm{disasm} {
  setSortingEnabled(true);
  connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(jumpToLabel(int, int)));
}

Q_DECLARE_METATYPE(Addr)

void LabelsWidget::refresh() {
  clear();
  setColumnCount(2);
  QStringList strlist;
  strlist<<tr("Name")<<tr("Address");
  setHorizontalHeaderLabels(strlist);
  setSortingEnabled(false);
  int i=0;
  Labels& my_labels=IDisassemblerCore::inst()->labels();
  setRowCount(my_labels.size());
  for(auto lbl: my_labels) {
      QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(lbl.second->name));
      setItem(i, 0, nameItem);
      QTableWidgetItem* addrItem = new QTableWidgetItem(QString::fromStdString(lbl.second->addr.toString()));
      setItem(i, 1, addrItem);
      QVariant var;
      var.setValue<Addr>(lbl.second->addr);
      nameItem->setData(Qt::UserRole, var);
      i++;
  }
  setSortingEnabled(true);
}

void LabelsWidget::jumpToLabel(int row, int) {
  Addr addr = item(row,0)->data(Qt::UserRole).value<Addr>();
  _disasm->navigateToAddress(addr);
}
