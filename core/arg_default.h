#ifndef ARG_DEFAULT_H
#define ARG_DEFAULT_H

#include "arg.h"

class ArgDefault : public Arg {
public:
  ArgDefault(std::string s) : value{s} {}
  std::string toString() {return value;}

  std::string value;
};

#endif // ARG_DEFAULT_H

