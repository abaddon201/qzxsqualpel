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

#include "widget_change_text.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

WidgetChangeText::WidgetChangeText(QWidget* par, QString title, QString label, QString old_text)
  : QDialog(par) {
  setWindowTitle(title);
  _text=new QLineEdit;
  _text->setText(old_text);
  QVBoxLayout* v=new QVBoxLayout();
  QHBoxLayout* h2=new QHBoxLayout();
  QLabel * l = new QLabel(label);
  h2->addWidget(l);
  h2->addWidget(_text);
  v->addLayout(h2);
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
