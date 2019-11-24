#ifndef ARG_H
#define ARG_H

#include <memory>
#include <utility>

#include "label.h"
#include "registers.h"
#include "flag.h"

namespace dasm {
namespace core {

enum class ArgType {
  ARG_DEFAULT,
  ARG_LABEL,
  ARG_FLAG,
  ARG_REGISTER8,
  ARG_REGISTER16,
  ARG_REGISTER_OFFSET,
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
class Argument {
public:
  virtual std::string toString() const = 0;
  virtual ~Argument() = default;
  ArgType arg_type;
};

using ArgPtr = std::shared_ptr<Argument>;

class ArgDefault : public Argument {
public:
  explicit ArgDefault(Byte& s, uint8_t bytes_length) : value{ (uint8_t)s }, bytes_length{ bytes_length } { arg_type = ArgType::ARG_DEFAULT; }
  //explicit ArgDefault(uint8_t s, uint8_t bytes_lenth = 1) : value{ s }, bytes_length{ bytes_length } { arg_type = ArgType::ARG_DEFAULT; }
  explicit ArgDefault(uint16_t s, uint8_t bytes_length) : value{ s }, bytes_length{ bytes_length } { arg_type = ArgType::ARG_DEFAULT; }
  virtual ~ArgDefault() = default;

  std::string toString() const override { return utils::hexify(value, bytes_length * 2); }
private:
  uint16_t value;
  uint8_t bytes_length;
};

class ArgLabel : public Argument {
public:
  ArgLabel(std::shared_ptr<Label> l) : label{ std::move(l) } { arg_type = ArgType::ARG_LABEL; }
  virtual ~ArgLabel() = default;

  std::string toString() const override { return label->name; }

  std::shared_ptr<Label> label;
};

class ArgFlag : public Argument {
public:
  ArgFlag(Flag f) : flag{ f } { arg_type = ArgType::ARG_FLAG; }
  virtual ~ArgFlag() = default;

  std::string toString() const override { return flag.toString(); }

  Flag flag;
};

class ArgRegister8 : public Argument {
public:
  ArgRegister8(Register8 reg_id) : reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER8; }
  virtual ~ArgRegister8() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register8 reg_id;
};

class ArgRegister16 : public Argument {
public:
  ArgRegister16(Register16 reg_id) : reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER16; }
  virtual ~ArgRegister16() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register16 reg_id;
};

class ArgRegisterOffset : public Argument {
public:
  ArgRegisterOffset(Register16 reg_id, const std::string& offs, bool is_positive) : reg_id{ reg_id }, is_positive{ is_positive } {
    arg_type = ArgType::ARG_REGISTER_OFFSET;
    offset = utils::hex2int(offs);
  }
  virtual ~ArgRegisterOffset() = default;

  std::string toString() const override { return "(" + reg_id.toString() + (is_positive ? "+" : "-") + utils::hexify(offset) + ")"; }

  Register16 reg_id;
  uint8_t offset;
  bool is_positive;
};

class ArgRegisterReference : public Argument {
public:
  ArgRegisterReference(Register16 reg_id) : reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER_REF; }
  virtual ~ArgRegisterReference() = default;

  std::string toString() const override { if (tstr_cache.empty()) { tstr_cache = "(" + reg_id.toString() + ")"; } return tstr_cache; }

  Register16 reg_id;

  mutable std::string tstr_cache;
};

class ArgMemoryReference : public Argument {
public:
  ArgMemoryReference(uint16_t addr) : addr{ addr } { arg_type = ArgType::ARG_MEMORY_REF; }
  virtual ~ArgMemoryReference() = default;

  std::string toString() const override { if (tstr_cache.empty()) { tstr_cache = "(ptr_" + utils::hexify(addr) + ")"; } return tstr_cache; }

  uint16_t addr;

  mutable std::string tstr_cache;
};

}
}

#endif // ARG_H

