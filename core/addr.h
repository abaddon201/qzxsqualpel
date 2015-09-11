//
// C++ Interface: CAddr
//
// Description:
//
//
// Author: Glebov Alex <abaddon@easi.ru>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CADDR_H
#define CADDR_H

#include <string>

class Addr {
public:
  Addr(unsigned long long offs=0, unsigned long long seg=0) 
    : _offset{offs}, _segment{seg}, _dirty{true} { }
  Addr(const Addr &rhs)            = default;
  Addr& operator=(const Addr& rhs) = default;
  virtual ~Addr()                  = default;

  unsigned long long offset() const { return _offset; }
  unsigned long long segment() const { return _segment; }

  bool compare(unsigned long long s) const { return _offset==s; }

  //prefix cases
  Addr &operator++() { _offset++; _dirty = true; return*this;}
  Addr &operator--() { _offset--; _dirty = true; return*this;}

  //postfix cases
  Addr operator++(int) { Addr tmp(*this); operator++(); return tmp; }
  Addr operator--(int) { Addr tmp(*this); operator--(); return tmp; }

  Addr &operator+=(const Addr &rhs) { _offset += rhs.offset(); _dirty = true; return *this; }
  Addr &operator-=(const Addr &rhs) { _offset -= rhs.offset(); _dirty = true; return *this; }

  //TODO: consider case to change operation overloadings below (they are not 
  //used currently) to functions i.e incOffset() :)
  Addr operator+(unsigned long long offs) const {return _offset+offs;}
  Addr operator-(unsigned long long offs) const {return _offset-offs;}
  Addr &operator+=(unsigned long long offs) { _offset+=offs; _dirty = true; return *this; }
  Addr &operator-=(unsigned long long offs) { _offset-=offs; _dirty = true; return *this; }

  const std::string& toString() const;
  std::string offsetString() const;

  friend bool operator< (const Addr &lhs, const Addr &rhs);
  friend bool operator==(const Addr &lhs, const Addr &rhs);
  friend Addr operator+ (Addr lhs, const Addr &rhs);
  friend Addr operator- (Addr lhs, const Addr &rhs);

private:
  unsigned long long _offset;
  unsigned long long _segment;
///@todo не всегда нужен кэш строки... Может стоит разбить на 2 класса
  mutable std::string _hex_cache;
  mutable bool _dirty;
};

//idiomatic way -- http://en.cppreference.com/w/cpp/language/operators
//example -- https://github.com/llvm-mirror/libcxx/blob/master/include/tuple#L949
///@bug не учитывается сегмент в сравнениях больше/меньше
inline bool operator<(const Addr &lhs, const Addr &rhs) {
  if (lhs.segment() != rhs.segment())
    return false;

  return lhs._offset < rhs._offset;
}

inline bool operator==(const Addr &lhs, const Addr &rhs) {
  if (lhs.segment() != rhs.segment())
    return false;

  return lhs._offset == rhs._offset;
}

inline bool operator> (const Addr &lhs, const Addr &rhs) { return rhs < lhs; }
inline bool operator>=(const Addr &lhs, const Addr &rhs) { return !(lhs < rhs);}
inline bool operator<=(const Addr &lhs, const Addr &rhs) { return !(lhs > rhs); } 
inline bool operator!=(const Addr &lhs, const Addr &rhs) { return !(lhs == rhs); }
inline Addr operator- (Addr lhs, const Addr &rhs) { lhs -= rhs; return lhs; }
inline Addr operator+ (Addr lhs, const Addr &rhs) { lhs += rhs; return lhs; }
#endif
