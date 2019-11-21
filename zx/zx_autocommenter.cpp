#include "zx_autocommenter.h"
#include "zx_jump_cmd.h"

//FIXME: load this data from file
void ZXAutoCommenter::commentCommand(dasm::core::Command &out_command) {
  if (out_command.command_code == (int) ZXJumpCmd::CMD_RST) {
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
  } else if (out_command.command_code == (int) ZXJumpCmd::CMD_CALL) {
    //known CALL's
    if (out_command.getJmpAddr() == 0x0556) {
      //Load File
      out_command.auto_comment = "Load File: IX=dst, DE=len, A=type";
    }
  }
}
