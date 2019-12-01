#ifndef ARG_H
#define ARG_H

#include <memory>
#include <utility>

#include "label.h"
#include "registers.h"
#include "flag.h"

namespace dasm {
namespace core {

enum class ArgSize : uint8_t {
  Flag = 0,
  Byte = 1,
  Word = 2
};

enum class ArgType : uint8_t {
  ARG_DEFAULT,
  //  ARG_LABEL,
  ARG_FLAG,
  ARG_REGISTER8,
  ARG_REGISTER16,
  ARG_REGISTER_OFFSET,
  ARG_REGISTER_REF,
  ARG_MEMORY_REF,
  ARG_PORT
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
  explicit Argument(ArgSize size) : size{ size } {}

  virtual std::string toString() const = 0;
  ArgSize getSize() const {
    return size;
  }

  void setSize(ArgSize sz) { size = sz; tstr_cache = ""; }

  int getBytesSize() const { return size == ArgSize::Byte ? 1 : 2; }

  virtual ~Argument() = default;

  ArgSize size;
  ArgType arg_type;
  mutable std::string tstr_cache;
};

using ArgPtr = std::shared_ptr<Argument>;

template<typename T>
std::shared_ptr<T> argConvert(ArgPtr ptr) {
  return std::static_pointer_cast<T>(ptr);
}

class ArgDefault : public Argument {
public:
  explicit ArgDefault(Byte& s) : Argument(ArgSize::Byte), _value{ (uint8_t)s }, _is_hex{ true } { arg_type = ArgType::ARG_DEFAULT; }
  explicit ArgDefault(uint16_t s, ArgSize bytes_length, bool is_hex) : Argument(bytes_length), _value{ s }, _is_hex{ _is_hex } { arg_type = ArgType::ARG_DEFAULT; }
  virtual ~ArgDefault() = default;

  std::string toString() const override { return _is_hex ? utils::hexify(_value, getBytesSize() * 2) : std::to_string(_value); }

  uint16_t value() const { return _value; }

  bool is_hex() const { return _is_hex; }

private:
  uint16_t _value;
  bool _is_hex;
};

using ArgDefaultPtr = std::shared_ptr<ArgDefault>;

class ArgByteArray : public Argument {
public:
  explicit ArgByteArray(int length) : Argument(ArgSize::Byte) {
    arg_type = ArgType::ARG_DEFAULT;
  }
  virtual ~ArgByteArray() = default;

  std::string toString() const override {
    if (tstr_cache.empty()) {
      auto it = bytes.begin();
      auto p_val = *it;
      ++it;
      int count = 1;
      bool isFirst = true;
      do {
        auto v = *it;
        if (v == p_val) {
          ++count;
        } else {
          if (isFirst) {
            isFirst = false;
          } else {
            tstr_cache += ", ";
          }
          if (count != 1) {
            tstr_cache += p_val.toString() + "(" + std::to_string(count) + ")";
          } else {
            tstr_cache += p_val.toString();
          }
          count = 1;
          p_val = v;
        }
        ++it;
      } while (it != bytes.end());
      if (isFirst) {
        isFirst = false;
      } else {
        tstr_cache += ", ";
      }
      if (count != 1) {
        tstr_cache += p_val.toString() + "(" + std::to_string(count) + ")";
      } else {
        tstr_cache += p_val.toString();
      }
    }
    return tstr_cache;
  }

  void pushByte(const Byte& b) {
    if (!bytes.empty()) {
      const auto& last = bytes.back();
      if (b == last) {
        //continue sequence
      } else {
        //finish sequence
      }
    }
    bytes.push_back(b);

  }

private:
  std::vector<Byte> bytes;
};

using ArgByteArrayPtr = std::shared_ptr<ArgByteArray>;

class ArgPort : public Argument {
public:
  explicit ArgPort(uint8_t s) : Argument(ArgSize::Byte), _value{ s } { arg_type = ArgType::ARG_PORT; }
  virtual ~ArgPort() = default;

  std::string toString() const override { return "(" + utils::hexify(_value, 2) + ")"; }

  uint16_t value() const { return _value; }

private:
  uint16_t _value;
};

/*class ArgLabel : public Argument {
public:
  ArgLabel(std::shared_ptr<Label> l) : label{ std::move(l) } { arg_type = ArgType::ARG_LABEL; }
  virtual ~ArgLabel() = default;

  std::string toString() const override { return label->name; }

  std::shared_ptr<Label> label;
};*/

class ArgFlag : public Argument {
public:
  ArgFlag(Flag f) : Argument(ArgSize::Flag), flag{ f } { arg_type = ArgType::ARG_FLAG; }
  virtual ~ArgFlag() = default;

  std::string toString() const override { return flag.toString(); }

  Flag flag;
};

class ArgRegister8 : public Argument {
public:
  ArgRegister8(Register8 reg_id) : Argument(ArgSize::Byte), reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER8; }
  virtual ~ArgRegister8() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register8 reg_id;
};

class ArgRegister16 : public Argument {
public:
  ArgRegister16(Register16 reg_id) : Argument(ArgSize::Word), reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER16; }
  virtual ~ArgRegister16() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register16 reg_id;
};

class ArgRegisterOffset : public Argument {
public:
  ArgRegisterOffset(Register16 reg_id, const std::string& offs, bool is_positive) : Argument(ArgSize::Byte), reg_id{ reg_id }, is_positive{ is_positive } {
    arg_type = ArgType::ARG_REGISTER_OFFSET;
    offset = (uint8_t)utils::hex2int(offs);
  }
  virtual ~ArgRegisterOffset() = default;

  std::string toString() const override { return "(" + reg_id.toString() + (is_positive ? "+" : "-") + utils::hexify(offset) + ")"; }

  Register16 reg_id;
  uint8_t offset;
  bool is_positive;
};

class ArgRegisterReference : public Argument {
public:
  ArgRegisterReference(Register16 reg_id) : Argument(ArgSize::Byte), reg_id{ reg_id } { arg_type = ArgType::ARG_REGISTER_REF; }
  virtual ~ArgRegisterReference() = default;

  std::string toString() const override { if (tstr_cache.empty()) { tstr_cache = "(" + reg_id.toString() + ")"; } return tstr_cache; }

  Register16 reg_id;
};

class ArgMemoryReference : public Argument {
public:
  ArgMemoryReference(uint16_t addr, bool isReference) : Argument(ArgSize::Byte), addr{ addr }, isReference{ isReference } { arg_type = ArgType::ARG_MEMORY_REF; }
  ArgMemoryReference(LabelPtr label, bool isReference) : Argument(ArgSize::Byte), label{ label }, isReference{ isReference } {
    arg_type = ArgType::ARG_MEMORY_REF;
    addr = label->addr.offset();
  }

  virtual ~ArgMemoryReference() = default;

  std::string toString() const override {
    if (tstr_cache.empty()) {
      if (isReference) {
        if (label != nullptr) {
          tstr_cache = std::string("(") + label->name + ")";
        } else {
          tstr_cache = std::string("(") + ((size == ArgSize::Byte) ? "b_" : "w_") + utils::hexify(addr) + ")";
        }
      } else {
        if (label != nullptr) {
          tstr_cache = label->name;
        } else {
          tstr_cache = ((size == ArgSize::Byte) ? "b_" : "w_") + utils::hexify(addr);
        }
      }
    }
    return tstr_cache;
  }

  void setLabel(LabelPtr label) { this->label = label; tstr_cache = ""; }

  bool isReference;
  uint16_t addr;
  LabelPtr label;
  mutable std::string tstr_cache;
};

}
}

#endif // ARG_H

