#include "autocommenter.h"

#include "core/cmd_code.h"
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
void AutoCommenter::commentCommand(dasm::core::CommandPtr out_command) {
  if (out_command->command_code == core::CmdCode::RST) {
    //known RST's
    if (std::dynamic_pointer_cast<core::ArgDefault>(out_command->getArg(0))->value() == 0x10) {
      out_command->auto_comment = "PRINT_A";
    } else if (std::dynamic_pointer_cast<core::ArgDefault>(out_command->getArg(0))->value() == 0x18) {
      out_command->auto_comment = "NEXT_CHAR";
    } else if (std::dynamic_pointer_cast<core::ArgDefault>(out_command->getArg(0))->value() == 0x20) {
      out_command->auto_comment = "GET_CHAR";
    } else if (std::dynamic_pointer_cast<core::ArgDefault>(out_command->getArg(0))->value() == 0x28) {
      out_command->auto_comment = "FP_CALC";
    }
  } else if (out_command->command_code == core::CmdCode::CALL) {
    //known CALL's
    /*if (out_command.getJmpAddr() == 0x0556) {
      //Load File
      out_command.auto_comment = "Load File: IX=dst, DE=len, A=type";
    }*/
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