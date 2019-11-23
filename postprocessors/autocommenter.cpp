#include "autocommenter.h"

#include "core/jump_type.h"
#include <iostream>
#include <fstream>

namespace dasm {
namespace postprocessors {

std::shared_ptr<core::Label> AutoCommenter::getLabelForAddr(const memory::Addr& addr) {
  auto it = _known_labels.find(addr);
  if (it != _known_labels.end()) {
    return it->second;
  }
  return nullptr;
}

//FIXME: load this data from file
void AutoCommenter::commentCommand(dasm::core::Command& out_command) {
  if (out_command.command_code == (int)core::JumpCmd::CMD_RST) {
    //known RST's
    if (out_command.arg1->toString() == "10") {
      out_command.auto_comment = "PRINT_A";
    } else if (out_command.arg1->toString() == "18") {
      out_command.auto_comment = "NEXT_CHAR";
    } else if (out_command.arg1->toString() == "20") {
      out_command.auto_comment = "GET_CHAR";
    } else if (out_command.arg1->toString() == "28") {
      out_command.auto_comment = "FP_CALC";
    }
  } else if (out_command.command_code == (int)core::JumpCmd::CMD_CALL) {
    //known CALL's
    if (out_command.getJmpAddr() == 0x0556) {
      //Load File
      out_command.auto_comment = "Load File: IX=dst, DE=len, A=type";
    }
  }
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
      memory::Addr a(off, seg);
      _known_labels[a] = std::make_shared<core::Label>(a, nm);
    }
  }
}

}
}