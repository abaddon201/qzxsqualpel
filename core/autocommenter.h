#pragma once

#include "command.h"
#include "chunk.h"
#include "labels.h"

namespace dasm {
namespace core {

class AutoCommenter {
public:
  virtual void commentCommand(Command& out_command) = 0;

  void loadGuessFile(const std::string& fname);

  LabelPtr getLabelForAddr(const Addr& addr);

  virtual ~AutoCommenter() = default;
private:
  ///@brief метки, подгруженные из внешнего файла
  Labels _known_labels;

};

}
}
