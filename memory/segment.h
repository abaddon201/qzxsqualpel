#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <stdexcept>
#include <algorithm>

#include "core/byte.h"

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

  Segment() : _size{ 0 }, _dataSize{ 0 }, _id{ 0 } {}

  Segment(IdType id, size_type sz) : _size{ sz }, _dataSize{ 0 }, _id{ id } {}

  void fill(unsigned char buff[], size_type size);

  inline IdType id() const { return _id; }

  inline size_type dataSize() const { return _dataSize; }

  inline core::Byte getByte(size_type offset) const {
    if (offset < _dataSize)
      return _mem[offset];
    else throw std::out_of_range("offset:" + std::to_string(offset) + ", max:" + std::to_string(_dataSize));
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
  std::vector<core::Byte> _mem;
};

}
}

#endif // SEGMENT_H
