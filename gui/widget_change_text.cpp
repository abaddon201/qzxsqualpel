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

namespace dasm {
namespace gui {

WidgetChangeText::WidgetChangeText(QWidget* par, QString title, QString label, QString old_text)
  : QDialog(par) {
  setWindowTitle(title);
  _text = new QTextEdit;
  _text->setText(old_text);
  QVBoxLayout* v = new QVBoxLayout();
  QLabel* l = new QLabel(label);
  v->addWidget(l);
  v->addWidget(_text);
  QHBoxLayout* h = new QHBoxLayout();
  QPushButton* bok = new QPushButton(tr("Ok"));
  QPushButton* bcanc = new QPushButton(tr("Cancel"));
  h->addWidget(bok);
  h->addWidget(bcanc);
  v->addLayout(h);
  setLayout(v);
  connect(bok, SIGNAL(clicked()), this, SLOT(accept()));
  connect(bcanc, SIGNAL(clicked()), this, SLOT(reject()));
}

}
}
