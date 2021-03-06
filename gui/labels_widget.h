//
// C++ Interface: CLabelsWidget
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CLABELSWIDGET_H
#define CLABELSWIDGET_H

#include <QTableWidget>
#include "disassembler_widget.h"

namespace dasm {
namespace gui {

class LabelsWidget : public QTableWidget {
  Q_OBJECT
public:
  LabelsWidget(QWidget* par, DisassemblerWidget* disasm);

  void refresh();

public slots:

  void jumpToLabel(int, int);

private:
  DisassemblerWidget* _disasm;
};

}
}
#endif
