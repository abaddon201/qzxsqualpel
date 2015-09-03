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

#include "CLabelsWidget.h"

#include "core/CLabel.h"
#include "CDisassemblerWidget.h"

CLabelsWidget::CLabelsWidget(QWidget* par, CDisassemblerWidget* disasm)
  : QTableWidget(par),
    _disasm{disasm} {
  setSortingEnabled(true);
  connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(jumpToLabel(int, int)));
}

Q_DECLARE_METATYPE(CAddr)

void CLabelsWidget::refresh() {
  clear();
  setColumnCount(2);
  QStringList strlist;
  strlist<<tr("Name")<<tr("Address");
  setHorizontalHeaderLabels(strlist);
  setSortingEnabled(false);
  int i=0;
  CLabels& my_labels=IDisassemblerCore::inst()->labels();
  setRowCount(my_labels.size());
  for(auto lbl: my_labels) {
      QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(lbl.name));
      setItem(i, 0, nameItem);
      QTableWidgetItem* addrItem = new QTableWidgetItem(QString::fromStdString(lbl.addr.toString()));
      setItem(i, 1, addrItem);
      QVariant var;
      var.setValue<CAddr>(lbl.addr);
      nameItem->setData(Qt::UserRole, var);
      i++;
  }
  setSortingEnabled(true);
}

void CLabelsWidget::jumpToLabel(int row, int) {
  CAddr addr = item(row,0)->data(Qt::UserRole).value<CAddr>();
  _disasm->navigateToAddress(addr);
}
