#ifndef DEBUG_PRINTERS_H
#define DEBUG_PRINTERS_H

#include "CLabel.h"
#include "chunk.h"

inline std::ostream &operator<<(std::ostream &out, Label &lab) {
  out<<"Label: Name='"<<lab.name<<"', addr="<<lab.addr.toString();
  return out;
}

inline std::ostream &operator<<(std::ostream &out, Chunk::Type t) {
  switch (t) {
  case Chunk::Type::UNKNOWN:
    out<<"UNKNOWN";
    break;
  case Chunk::Type::UNPARSED:
    out<<"UNPARSED";
    break;
  case Chunk::Type::CODE:
    out<<"CODE";
    break;
  case Chunk::Type::DATA_BYTE:
    out<<"DATA_BYTE";
    break;
  case Chunk::Type::DATA_WORD:
    out<<"DATA_WORD";
    break;
  case Chunk::Type::DATA_ARRAY:
    out<<"DATA_ARRAY";
    break;
  }
  return out;
}

///@bug total shit
//#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // DEBUG_PRINTERS_H

