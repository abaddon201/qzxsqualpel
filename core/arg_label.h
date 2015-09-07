#ifndef ARG_LABEL_H
#define ARG_LABEL_H

#include <memory>

#include "arg.h"
#include "CLabel.h"

class ArgLabel : public Arg {
public:
  std::string toString() override {return label->name;}
  ArgLabel(std::shared_ptr<CLabel> l) : label{l} {}
  virtual ~ArgLabel() {}

  std::shared_ptr<CLabel> label;
};

#endif // ARG_LABEL_H

