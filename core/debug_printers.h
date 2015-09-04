#ifndef DEBUG_PRINTERS_H
#define DEBUG_PRINTERS_H

#include "CLabel.h"
#include "CChunk.h"

inline std::ostream &operator<<(std::ostream &out, CLabel &lab) {
  out<<"Label: Name='"<<lab.name<<"', addr="<<lab.addr.toString();
  return out;
}

inline std::ostream &operator<<(std::ostream &out, CChunk::Type t) {
  switch (t) {
  case CChunk::Type::UNKNOWN:
    out<<"UNKNOWN";
    break;
  case CChunk::Type::UNPARSED:
    out<<"UNPARSED";
    break;
  case CChunk::Type::CODE:
    out<<"CODE";
    break;
  case CChunk::Type::DATA_BYTE:
    out<<"DATA_BYTE";
    break;
  case CChunk::Type::DATA_WORD:
    out<<"DATA_WORD";
    break;
  case CChunk::Type::DATA_ARRAY:
    out<<"DATA_ARRAY";
    break;
  }
  return out;
}

///@bug total shit
//#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // DEBUG_PRINTERS_H

