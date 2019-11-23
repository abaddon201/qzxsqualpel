#ifndef ARG_H
#define ARG_H

#include <memory>
#include <utility>

#include "label.h"
#include "registers.h"

namespace dasm {
namespace core {

enum class ArgType {
  ARG_DEFAULT,
  ARG_LABEL,
  ARG_REGISTER,
  ARG_REGISTER_REF,
  ARG_MEMORY_REF
};
/*
enum class ArgType {
  ARG_UNKNOWN = 0,
  ARG_REGISTER = 1,
  ARG_DATAOFFSET = 2,
  ARG_JUMPOFFSET = 3,
  ARG_VALUE = 4,
  ARG_VALOFFSET = 5,
  ARG_FLAG = 6
};*/

///@brief базовый класс для аргументов
class Arg {
public:
  virtual std::string toString() const = 0;
  virtual ~Arg() = default;
  ArgType arg_type;
};

class ArgDefault : public Arg {
public:
  explicit ArgDefault(std::string& s) : value{ std::move(s) } { arg_type = ArgType::ARG_DEFAULT; }
  virtual ~ArgDefault() = default;

  std::string toString() const override { return value; }

  std::string value;
};

class ArgLabel : public Arg {
public:
  ArgLabel(std::shared_ptr<Label> l) : label{ std::move(l) } { arg_type = ArgType::ARG_LABEL; }
  virtual ~ArgLabel() = default;

  std::string toString() const override { return label->name; }

  std::shared_ptr<Label> label;
};

class ArgRegisterReference : public Arg {
public:
  ArgRegisterReference(Register16 reg_id) : reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER_REF; }
  virtual ~ArgRegisterReference() = default;

  std::string toString() const override { if (tstr_cache.empty()) { tstr_cache = "(" + reg_id.toString() + ")"; } return tstr_cache; }

  Register16 reg_id;

  mutable std::string tstr_cache;
};

}
}

#endif // ARG_H

