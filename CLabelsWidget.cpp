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

#include "CLabel.h"
#include "CDisassembler.h"

CLabelsWidget::CLabelsWidget(QWidget* par)
  : QTableWidget(par) {
  setSortingEnabled(true);
  connect(this, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(jumpToLabel(int, int)));
}

void CLabelsWidget::refresh() {
  clear();
  QList<CLabel> my_labels=CDisassembler::core_inst()->labels();
  setRowCount(my_labels.count());
  setColumnCount(2);
  int i=0;
  QStringList strlist;
  strlist<<tr("Name")<<tr("Address");
  setHorizontalHeaderLabels(strlist);
  setSortingEnabled(false);
  if (my_labels.count()) {
    QList<CLabel>::const_iterator it;
    for (it=my_labels.begin(); it!=my_labels.end(); ++it) {
      CLabel lbl=*it;
      QTableWidgetItem* nameItem = new QTableWidgetItem(lbl.name);
      setItem(i, 0, nameItem);
      QTableWidgetItem* addrItem = new QTableWidgetItem(lbl.addr.toString());
      setItem(i, 1, addrItem);
      i++;
    }
  }
  setSortingEnabled(true);
}

void CLabelsWidget::jumpToLabel(int row, int) {
  CDisassembler::inst()->navigateToLabel(row);
}
