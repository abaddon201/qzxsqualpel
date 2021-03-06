#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <string>

namespace dasm {
namespace memory {

///@brief Описание сегмента памяти
class Segment {
public:
  using IdType = unsigned long long;
  using size_type = size_t;

  enum class Type {
    ///@brief Неопознаный тип
    RAW,
    ///@brief Кодовый сегмент
    CODE,
    ///@brief Сегмент данных
    DATA
  };

  Segment() : _size{ 0 }, _dataSize{ 0 }, _id{ 0 }, _type{ Type::RAW } {}

  Segment(IdType id, size_type sz) : _size{ sz }, _dataSize{ 0 }, _id{ id }, _type{ Type::RAW } {}

  void fill(unsigned char buff[], size_type size);

  inline IdType id() const { return _id; }
  inline void setId(IdType id) { _id = id; }

  inline size_type dataSize() const { return _dataSize; }
  inline void setDataSize(size_type sz) { _dataSize = sz; }

  inline size_type size() const { return _size; }
  inline void setSize(size_type sz) { _size = sz; }

  inline Type type() const { return _type; }
  inline void setType(Type type) { _type = type; }

  const std::vector<uint8_t>& bytes() const { return _mem; }
  std::vector<uint8_t>& bytes() { return _mem; }

  inline uint8_t getByte(size_type offset) const {
    if (offset < _dataSize) {
      return _mem[offset];
    } else {
      throw std::out_of_range("offset:" + std::to_string(offset) + ", max:" + std::to_string(_dataSize));
    }
  }

  inline void setByte(size_type offset, uint8_t b) {
    if (offset < _dataSize) {
      _mem[offset] = b;
    } else {
      throw std::out_of_range("offset:" + std::to_string(offset) + ", max:" + std::to_string(_dataSize));
    }
  }

private:
  ///@brief размер сегмента в байтах
  size_type _size;
  ///@brief размер данных в сегменте в байтах
  size_type _dataSize;
  ///@brief идентификатор сегмента (дескриптор/номер/адрес в памяти)
  IdType _id;
  ///@brief Тип сегмента
  Type _type;
  ///@brief Содержимое сегмента
  std::vector<uint8_t> _mem;
};

using SegmentPtr = std::shared_ptr<Segment>;
}
}

#endif // SEGMENT_H
