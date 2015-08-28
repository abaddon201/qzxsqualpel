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

void CLabelsWidget::refresh() {
  clear();
  CLabels my_labels=IDisassemblerCore::inst()->labels();
  setRowCount(my_labels.size());
  setColumnCount(2);
  int i=0;
  QStringList strlist;
  strlist<<tr("Name")<<tr("Address");
  setHorizontalHeaderLabels(strlist);
  setSortingEnabled(false);
  if (my_labels.size()) {
    CLabels::const_iterator it;
    for (it=my_labels.begin(); it!=my_labels.end(); ++it) {
      CLabel lbl=*it;
      QTableWidgetItem* nameItem = new QTableWidgetItem(QString::fromStdString(lbl.name));
      setItem(i, 0, nameItem);
      QTableWidgetItem* addrItem = new QTableWidgetItem(QString::fromStdString(lbl.addr.toString()));
      setItem(i, 1, addrItem);
      i++;
    }
  }
  setSortingEnabled(true);
}

void CLabelsWidget::jumpToLabel(int row, int) {
  _disasm->navigateToLabel(row);
}
