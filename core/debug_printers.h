#ifndef DEBUG_PRINTERS_H
#define DEBUG_PRINTERS_H

#include "CLabel.h"
#include "CChunk.h"

///@todo Debug shit must thing about cut this qt from here
#include <QDebug>
inline QDebug operator<<(QDebug out, std::string str) {
  out<<QString::fromStdString(str);
  return out;
}

template<typename T>
inline QDebug operator<<(QDebug out, std::vector<T> v) {
  for(auto val:v) {
    ///@bug recursion
    //out<<val<<", ";
  }
  return out;
}

inline void operator<<(QDebug out, CLabel lab) {
  out<<"Label: Name='"<<lab.name<<"', addr="<<lab.addr.toString();
}

inline QDebug operator<<(QDebug out, CChunk::Type t) {
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
#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // DEBUG_PRINTERS_H

