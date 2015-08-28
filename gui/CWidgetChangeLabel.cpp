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

#include "CWidgetChangeLabel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

CWidgetChangeLabel::CWidgetChangeLabel(QWidget* par, QString old_link)
  : QDialog(par) {
  m_LinkName=new QLineEdit;
  m_LinkName->setText(old_link);
  QVBoxLayout* v=new QVBoxLayout();
  v->addWidget(m_LinkName);
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
