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

class WidgetChangeText : public QDialog {
  Q_OBJECT
public:
  WidgetChangeText(QWidget* par, QString old_text);
  QString text() { return _text->text(); }

private:
  QLineEdit* _text;
};

#endif
