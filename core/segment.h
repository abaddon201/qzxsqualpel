#ifndef SEGMENT_H
#define SEGMENT_H

#include <vector>
#include <stdexcept>
#include <algorithm>

#include "byte.h"

///@brief Описание сегмента памяти
class Segment {
public:
  using IdType = unsigned long long;

  enum class Type {
    ///@brief Неопознаный тип
    RAW,
    ///@brief Кодовый сегмент
    CODE,
    ///@brief Сегмент данных
    DATA
  };

  Segment() : _size{0}, _dataSize{0}, _id{0} {}
  Segment(IdType id, unsigned long long sz) : _size{sz}, _dataSize{0}, _id{id} {}
  void fill(unsigned char buff[], unsigned long long size);

  inline IdType id() const {return _id;}
  inline unsigned long long dataSize() const {return _dataSize;}

  inline Byte getByte(unsigned long long offset) const {if (offset<_dataSize) return _mem[offset]; else throw std::out_of_range("offset:"+std::to_string(offset)+", max:"+std::to_string(_dataSize));}

private:
  ///@brief размер сегмента в байтах
  unsigned long long _size;
  ///@brief размер данных в сегменте в байтах
  unsigned long long _dataSize;
  ///@brief идентификатор сегмента (дескриптор/номер/адрес в памяти)
  IdType _id;
  ///@brief Тип сегмента
  Type _type;
  ///@brief Содержимое сегмента
  std::vector<unsigned char> _mem;
};

#endif // SEGMENT_H
