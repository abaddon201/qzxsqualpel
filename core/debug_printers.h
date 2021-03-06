#ifndef DEBUG_PRINTERS_H
#define DEBUG_PRINTERS_H

#include "label.h"
#include "utils/utils.h"

using namespace dasm::core;

inline std::ostream &operator<<(std::ostream &out, Label &lab) {
  out << "Label: Name='" << lab.name << "', addr=" << dasm::utils::toHex(lab.addr);
  return out;
}

///@bug total shit
//#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // DEBUG_PRINTERS_H

