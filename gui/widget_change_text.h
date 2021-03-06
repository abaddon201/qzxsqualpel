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

#ifndef CWIDGETCHANGETEXT_H
#define CWIDGETCHANGETEXT_H

#include <QDialog>
#include <QTextEdit>

namespace dasm {
namespace gui {

class WidgetChangeText : public QDialog {
  Q_OBJECT
public:
  WidgetChangeText(QWidget* par, QString title, QString label, QString old_text);

  QString text() { return _text->document()->toPlainText(); }

private:
  QTextEdit* _text;
};

}
}
#endif
