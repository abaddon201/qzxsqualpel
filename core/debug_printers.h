#ifndef DEBUG_PRINTERS_H
#define DEBUG_PRINTERS_H

#include "label.h"

using namespace dasm::core;

inline std::ostream &operator<<(std::ostream &out, Label &lab) {
  out << "Label: Name='" << lab.name << "', addr=" << lab.addr.toString();
  return out;
}

///@bug total shit
//#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // DEBUG_PRINTERS_H

