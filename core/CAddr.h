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

class CAddr {
public:
  CAddr(unsigned long long offs=0, unsigned long long seg=0) : m_Offset{offs}, m_Segment{seg} {}

  unsigned long long offset() const { return m_Offset; }
  unsigned long long segment() const { return m_Segment; }

  bool compare(unsigned long long s) const { return m_Offset==s; }
  bool operator==(const CAddr &s) const { return ((m_Offset==s.m_Offset) && (m_Segment==s.m_Segment)); }
  bool operator!=(const CAddr &s) const { return ((m_Offset!=s.m_Offset) || (m_Segment!=s.m_Segment)); }
  ///@bug не учитывается сегмент в сравнениях больше/меньше
  bool operator>=(const CAddr &s) const { return ((m_Offset>=s.m_Offset) && (m_Segment==s.m_Segment)); }
  bool operator<=(const CAddr &s) const { return ((m_Offset<=s.m_Offset) && (m_Segment==s.m_Segment)); }
  bool operator>(const CAddr &s) const { return ((m_Offset>s.m_Offset) && (m_Segment==s.m_Segment)); }
  bool operator<(const CAddr &s) const { return ((m_Offset>s.m_Offset) && (m_Segment==s.m_Segment)); }

  CAddr &operator++() { m_Offset++; return*this;}
  CAddr &operator--() { m_Offset--; return*this;}

  CAddr operator+(unsigned long long offs) const {return m_Offset+offs;}
  CAddr operator-(unsigned long long offs) const {return m_Offset-offs;}
  CAddr operator-(const CAddr &raddr) const { return m_Offset-raddr.m_Offset; }
  CAddr operator+(const CAddr &raddr) const { return m_Offset+raddr.m_Offset; }

  CAddr &operator+=(unsigned long long offs) { m_Offset+=offs; return *this; }
  CAddr &operator-=(unsigned long long offs) { m_Offset-=offs; return *this; }

  std::string toString() const;
  std::string offsetString() const;

private:
  unsigned long long m_Offset;
  unsigned long long m_Segment;
};

#endif
