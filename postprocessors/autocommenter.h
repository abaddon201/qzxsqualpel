#pragma once

#include "core/command.h"
#include "core/chunk.h"
#include "core/labels.h"

namespace dasm {
namespace postprocessors {

class AutoCommenter {
public:
  void commentCommand(core::CommandPtr out_command);

  void loadGuessFile(const std::string& fname);

  core::LabelPtr getLabelForAddr(const memory::Addr& addr);

  const core::Labels& getKnownLabels() const { return _known_labels; }

  core::Labels& knownLabels() { return _known_labels; }

  virtual ~AutoCommenter() = default;
private:
  ///@brief метки, подгруженные из внешнего файла
  core::Labels _known_labels;

};

}
}
