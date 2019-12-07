#include "document_helper.h"

#include "utils/utils.h"

#include <QTextCursor>
#include <QTextBlock>

#include <QDebug>
#include "utils/qt_debug_printers.h"

namespace dasm {
namespace gui {

std::string DocumentHelper::getString(int pos, int count) const {
  QString res{};
  for (int i = 0; i < count; ++i) {
    res += _doc->document()->characterAt(pos + i);
  }
  return res.toStdString();
}

std::string DocumentHelper::getWordUnderCursor() {
  QTextCursor cursor(_doc->textCursor());
  cursor.select(QTextCursor::WordUnderCursor);
  auto txt = cursor.selectedText();
  auto sel_strt = cursor.selectionStart() - 1;
  auto sel_end = cursor.selectionEnd();
  while (sel_strt >= 0 && !_doc->document()->characterAt(sel_strt).isSpace()) {
    //check syms left
    auto c = _doc->document()->characterAt(sel_strt);
    txt = c + txt;
    sel_strt--;
  }
  auto fin = sel_end + 30;
  while (sel_end < fin && !_doc->document()->characterAt(sel_end).isSpace()) {
    //check syms left
    auto c = _doc->document()->characterAt(sel_end);
    txt += c;
    ++sel_end;
  }
  qDebug() << "GUI: navigate to reference:" << cursor.block().text();
  qDebug() << "GUI: text:" << txt.toStdString();
  qDebug() << "GUI: Cursor pos:" << cursor.position();
  return txt.toStdString();
}

}
}
