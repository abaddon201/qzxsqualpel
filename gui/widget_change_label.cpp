//
// C++ Implementation: CWidgetChangeLabel
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "widget_change_label.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

WidgetChangeLabel::WidgetChangeLabel(QWidget* par, QString old_link)
  : QDialog(par) {
  _link_name=new QLineEdit;
  _link_name->setText(old_link);
  QVBoxLayout* v=new QVBoxLayout();
  v->addWidget(_link_name);
  QHBoxLayout* h=new QHBoxLayout();
  QPushButton* bok=new QPushButton(tr("Ok"));
  QPushButton* bcanc=new QPushButton(tr("Cancel"));
  h->addWidget(bok);
  h->addWidget(bcanc);
  v->addLayout(h);
  setLayout(v);
  connect(bok, SIGNAL(clicked()), this, SLOT(accept()));
  connect(bcanc, SIGNAL(clicked()), this, SLOT(reject()));
}
