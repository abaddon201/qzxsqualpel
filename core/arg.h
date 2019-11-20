#ifndef ARG_H
#define ARG_H

enum ArgTypes {
  ARG_DEFAULT,
  ARG_LABEL,
  ARG_REGISTER
};

///@brief базовый класс для аргументов
class Arg {
public:
  virtual std::string toString() = 0;
  virtual ~Arg() = default;
  int arg_type;
};

class ArgDefault : public Arg {
public:
  explicit ArgDefault(std::string s) : value{std::move(s)} {arg_type=ARG_DEFAULT;}
  virtual ~ArgDefault() = default;

  std::string toString() override {return value;}

  std::string value;
};

#include <memory>
#include <utility>
#include "label.h"

class ArgLabel : public Arg {
public:
  ArgLabel(std::shared_ptr<Label> l) : label{std::move(l)} {arg_type=ARG_LABEL;}
  virtual ~ArgLabel() = default;

  std::string toString() override {return label->name;}

  std::shared_ptr<Label> label;
};

#endif // ARG_H

