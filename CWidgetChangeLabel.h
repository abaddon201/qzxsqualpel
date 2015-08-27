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

class CWidgetChangeLabel : public QDialog {
  Q_OBJECT
public:
  CWidgetChangeLabel(QWidget* par, QString old_link);
  QString label() {
    return m_LinkName->text();
  }
private:
  QLineEdit* m_LinkName;
};

#endif
