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
  Addr(unsigned long long offs=0, unsigned long long seg=0) : _offset{offs}, _segment{seg}, _dirty{true} {}

  unsigned long long offset() const { return _offset; }
  unsigned long long segment() const { return _segment; }

  bool compare(unsigned long long s) const { return _offset==s; }
  bool operator==(const Addr &s) const { return ((_offset==s._offset) && (_segment==s._segment)); }
  bool operator!=(const Addr &s) const { return ((_offset!=s._offset) || (_segment!=s._segment)); }
  ///@bug не учитывается сегмент в сравнениях больше/меньше
  bool operator>=(const Addr &s) const { return ((_offset>=s._offset) && (_segment==s._segment)); }
  bool operator<=(const Addr &s) const { return ((_offset<=s._offset) && (_segment==s._segment)); }
  bool operator>(const Addr &s) const { return ((_offset>s._offset) && (_segment==s._segment)); }
  bool operator<(const Addr &s) const { return ((_offset<s._offset) && (_segment==s._segment)); }

  Addr &operator++() { _offset++; _dirty = true; return*this;}
  Addr &operator--() { _offset--; _dirty = true; return*this;}

  Addr operator+(unsigned long long offs) const {return _offset+offs;}
  Addr operator-(unsigned long long offs) const {return _offset-offs;}
  Addr operator-(const Addr &raddr) const { return _offset-raddr._offset; }
  Addr operator+(const Addr &raddr) const { return _offset+raddr._offset; }

  Addr &operator+=(unsigned long long offs) { _offset+=offs; _dirty = true; return *this; }
  Addr &operator-=(unsigned long long offs) { _offset-=offs; _dirty = true; return *this; }

  const std::string& toString() const;
  std::string offsetString() const;

private:
  unsigned long long _offset;
  unsigned long long _segment;
///@todo не всегда нужен кэш строки... Может стоит разбить на 2 класса
  mutable std::string _hex_cache;
  mutable bool _dirty;
};

#endif
