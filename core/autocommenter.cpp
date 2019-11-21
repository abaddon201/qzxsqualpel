#include "autocommenter.h"

#include <iostream>
#include <fstream>

namespace dasm {
namespace core {

std::shared_ptr<Label> AutoCommenter::getLabelForAddr(const Addr& addr) {
  auto it = _known_labels.find(addr);
  if (it != _known_labels.end()) {
    return it->second;
  }
  return nullptr;
}

void AutoCommenter::loadGuessFile(const std::string& fname) {
  std::ifstream f(fname);
  while (!f.eof()) {
    unsigned long long seg, off;
    std::string nm;
    f >> std::hex >> seg;
    f.ignore(1);
    f >> std::hex >> off;
    f >> nm;
    if (!nm.empty()) {
      Addr a(off, seg);
      _known_labels[a] = std::make_shared<Label>(a, nm);
    }
  }
}

}
}