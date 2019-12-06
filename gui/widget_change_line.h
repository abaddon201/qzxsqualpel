//
// C++ Interface: CWidgetChangeLabel
//
// Description:
//
//
// Author: Glebov Alex <aglebov2@gmail.com>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CWIDGETCHANGELINE_H
#define CWIDGETCHANGELINE_H

#include <QDialog>
#include <QLineEdit>

namespace dasm {
namespace gui {

class WidgetChangeLine : public QDialog {
  Q_OBJECT
public:
  WidgetChangeLine(QWidget* par, QString title, QString label, QString old_text);

  QString text() { return _text->text(); }

private:
  QLineEdit* _text;
};

}
}
#endif
