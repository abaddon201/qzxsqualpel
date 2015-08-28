//
// C++ Interface: CLabelsWidget
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABELSWIDGET_H
#define CLABELSWIDGET_H

#include <QTableWidget>
#include "CDisassemblerWidget.h"

class CLabelsWidget : public QTableWidget {
  Q_OBJECT
public:
  CLabelsWidget(QWidget* par, CDisassemblerWidget* disasm);
  void refresh();
public slots:
  void jumpToLabel(int, int);
private:
  CDisassemblerWidget* _disasm;
};

#endif
