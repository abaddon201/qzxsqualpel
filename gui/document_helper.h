#pragma once

#include <string>
#include <QPlainTextEdit>

namespace dasm {
namespace gui {

class DocumentHelper {
public:

  std::string getString(int pos, int count) const;

  uint16_t getAddrFromLineStart() const;

  std::string getWordUnderCursor();


  static DocumentHelper& inst() {
    static DocumentHelper _inst;
    return _inst;
  }

  void init(QPlainTextEdit* doc) {
    _doc = doc;
  }
private:
  DocumentHelper() = default;
  QPlainTextEdit* _doc;
};
}
}