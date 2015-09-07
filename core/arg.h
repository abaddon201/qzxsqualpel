#ifndef ARG_H
#define ARG_H

///@brief базовый класс для аргументов
class Arg {
public:
  virtual std::string toString() = 0;
  virtual ~Arg() {}
};

#endif // ARG_H

