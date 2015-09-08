//
// C++ Interface: CWidgetChangeLabel
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CWIDGETCHANGELABEL_H
#define CWIDGETCHANGELABEL_H

#include <QDialog>
#include <QLineEdit>

class WidgetChangeLabel : public QDialog {
  Q_OBJECT
public:
  WidgetChangeLabel(QWidget* par, QString old_link);
  QString label() { return _link_name->text(); }

private:
  QLineEdit* _link_name;
};

#endif
