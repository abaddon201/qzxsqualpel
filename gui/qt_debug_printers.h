#ifndef QT_DEBUG_PRINTERS_H
#define QT_DEBUG_PRINTERS_H

inline QDebug operator<<(QDebug out, std::string str) {
  out<<QString::fromStdString(str);
  return out;
}

template<typename T>
inline QDebug operator<<(QDebug out, std::vector<T> v) {
  for (auto val:v) {
    ///@bug recursion
    //out<<val<<", ";
  }
  return out;
}

#define Debug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // QT_DEBUG_PRINTERS_H

