#ifndef ARG_H
#define ARG_H

#include <memory>
#include <utility>

#include "label.h"
#include "registers.h"
#include "flag.h"
#include "utils/utils.h"

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
  ARG_PORT,
  ARG_BYTE_ARRAY
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
  explicit Argument(ArgType type, ArgSize size) : size{ size }, arg_type{ type } {}

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
  explicit ArgDefault() : Argument(ArgType::ARG_DEFAULT, ArgSize::Byte) {}
  explicit ArgDefault(uint8_t s) : Argument(ArgType::ARG_DEFAULT, ArgSize::Byte), _value{ s }, _is_hex{ true } { }
  explicit ArgDefault(uint16_t s, ArgSize bytes_length, bool is_hex) : Argument(ArgType::ARG_DEFAULT, bytes_length), _value{ s }, _is_hex{ _is_hex } { }
  virtual ~ArgDefault() = default;

  std::string toString() const override { return _is_hex ? utils::toHex(_value, getBytesSize()) : std::to_string(_value); }

  uint16_t value() const { return _value; }
  void setValue(uint16_t v) { _value = v; }

  bool is_hex() const { return _is_hex; }
  void setIsHex(bool ih) { _is_hex = ih; }

private:
  uint16_t _value;
  bool _is_hex;
};

using ArgDefaultPtr = std::shared_ptr<ArgDefault>;

class ArgByteArray : public Argument {
public:
  explicit ArgByteArray() : Argument(ArgType::ARG_BYTE_ARRAY, ArgSize::Byte) {}
  explicit ArgByteArray(int length) : Argument(ArgType::ARG_BYTE_ARRAY, ArgSize::Byte) {}
  virtual ~ArgByteArray() = default;

  std::string toString() const override {
    if (tstr_cache.empty()) {
      auto it = _bytes.begin();
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
            tstr_cache += utils::toHex(p_val) + "(" + std::to_string(count) + ")";
          } else {
            tstr_cache += utils::toHex(p_val);
          }
          count = 1;
          p_val = v;
        }
        ++it;
      } while (it != _bytes.end());
      if (isFirst) {
        isFirst = false;
      } else {
        tstr_cache += ", ";
      }
      if (count != 1) {
        tstr_cache += utils::toHex(p_val) + "(" + std::to_string(count) + ")";
      } else {
        tstr_cache += utils::toHex(p_val);
      }
    }
    return tstr_cache;
  }

  void pushByte(uint8_t b) {
    if (!_bytes.empty()) {
      const auto& last = _bytes.back();
      if (b == last) {
        //continue sequence
      } else {
        //finish sequence
      }
    }
    _bytes.push_back(b);
  }

  const std::vector<uint8_t>& bytes() const { return _bytes; }
private:
  std::vector<uint8_t> _bytes;
};

using ArgByteArrayPtr = std::shared_ptr<ArgByteArray>;

class ArgPort : public Argument {
public:
  ArgPort() : Argument(ArgType::ARG_PORT, ArgSize::Byte) {}
  explicit ArgPort(uint8_t s) : Argument(ArgType::ARG_PORT, ArgSize::Byte), _value{ s } {  }
  virtual ~ArgPort() = default;

  std::string toString() const override { return "(" + utils::toHex(_value) + ")"; }

  uint8_t value() const { return _value; }
  void setPort(uint16_t p) { _value = p; }
private:
  uint8_t _value;
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
  ArgFlag() : Argument(ArgType::ARG_FLAG, ArgSize::Flag), flag{ Flag::None } {}
  explicit ArgFlag(Flag f) : Argument(ArgType::ARG_FLAG, ArgSize::Flag), flag{ f } { }
  virtual ~ArgFlag() = default;

  std::string toString() const override { return flag.toString(); }

  Flag flag;
};

class ArgRegister8 : public Argument {
public:
  ArgRegister8() : Argument(ArgType::ARG_REGISTER8, ArgSize::Byte), reg_id{ Register8::None } {}
  explicit ArgRegister8(Register8 reg_id) : Argument(ArgType::ARG_REGISTER8, ArgSize::Byte), reg_id{ reg_id } { }
  virtual ~ArgRegister8() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register8 reg_id;
};

class ArgRegister16 : public Argument {
public:
  ArgRegister16() : Argument(ArgType::ARG_REGISTER16, ArgSize::Word), reg_id{ Register16::None } {}
  explicit ArgRegister16(Register16 reg_id) : Argument(ArgType::ARG_REGISTER16, ArgSize::Word), reg_id{ reg_id } { }
  virtual ~ArgRegister16() = default;

  std::string toString() const override { return reg_id.toString(); }

  Register16 reg_id;
};

class ArgRegisterOffset : public Argument {
public:
  ArgRegisterOffset() : Argument(ArgType::ARG_REGISTER_OFFSET, ArgSize::Byte), reg_id{ Register16::None } {}
  explicit ArgRegisterOffset(Register16 reg_id, const std::string& offs, bool is_positive) : Argument(ArgType::ARG_REGISTER_OFFSET, ArgSize::Byte), reg_id{ reg_id }, is_positive{ is_positive } {
    offset = (uint8_t)utils::fromHex(offs);
  }
  virtual ~ArgRegisterOffset() = default;

  std::string toString() const override { return "(" + reg_id.toString() + (is_positive ? "+" : "-") + utils::toHex(offset) + ")"; }

  Register16 reg_id;
  uint8_t offset;
  bool is_positive;
};

class ArgRegisterReference : public Argument {
public:
  ArgRegisterReference() : Argument(ArgType::ARG_REGISTER_REF, ArgSize::Byte), reg_id{ Register16::None } {}
  explicit ArgRegisterReference(Register16 reg_id) : Argument(ArgType::ARG_REGISTER_REF, ArgSize::Byte), reg_id{ reg_id } { }
  virtual ~ArgRegisterReference() = default;

  std::string toString() const override { if (tstr_cache.empty()) { tstr_cache = "(" + reg_id.toString() + ")"; } return tstr_cache; }

  Register16 reg_id;
};

class ArgMemoryReference : public Argument {
public:
  ArgMemoryReference() : Argument(ArgType::ARG_MEMORY_REF, ArgSize::Byte) {}
  ArgMemoryReference(uint16_t addr, bool isReference) : Argument(ArgType::ARG_MEMORY_REF, ArgSize::Byte), addr{ addr }, isReference{ isReference } { }
  ArgMemoryReference(LabelPtr label, bool isReference) : Argument(ArgType::ARG_MEMORY_REF, ArgSize::Byte), label{ label }, isReference{ isReference } {
    addr = label->addr;
  }

  virtual ~ArgMemoryReference() = default;

  std::string toString() const override {
    if (tstr_cache.empty()) {
      if (isReference) {
        if (label != nullptr) {
          tstr_cache = std::string("(") + label->name + ")";
        } else {
          tstr_cache = std::string("(") + ((size == ArgSize::Byte) ? "b_" : "w_") + utils::toHex(addr) + ")";
        }
      } else {
        if (label != nullptr) {
          tstr_cache = label->name;
        } else {
          tstr_cache = ((size == ArgSize::Byte) ? "b_" : "w_") + utils::toHex(addr);
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

