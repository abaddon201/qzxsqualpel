#include "register_reference_scan.h"

#include "core/cmd_code.h"
#include "core/disassembler_core.h"

namespace dasm {
using namespace core;
namespace postprocessors {

void RegisterReferenceScan::updateRegisterSource(memory::Addr& addr, ArgPtr arg, ArgSize size) {
  if (arg->arg_type != ArgType::ARG_REGISTER_REF) {
    return;
  }
  auto arg_ref = std::static_pointer_cast<ArgRegisterReference>(arg);
  auto reg = arg_ref->reg_id;
  updateRegisterSource(addr, reg, size);
}

void RegisterReferenceScan::updateRegisterSource(memory::Addr& addr, Register16 reg, ArgSize size) {
  memory::Addr tmp_addr = addr;
  auto commands = core::DisassemblerCore::inst().commands();
  CommandPtr cmd;
  do {
    cmd = commands.get(tmp_addr.offset());
    //TODO also check register halves
    if (cmd->getArgsCount() > 0) {
      if (cmd->getArg(0)->arg_type == ArgType::ARG_REGISTER16) {
        auto tst_ref = std::static_pointer_cast<ArgRegister16>(cmd->getArg(0));
        if (reg == tst_ref->reg_id) {
          // found register load, need to update right arg (if it's applicable)
          if (cmd->command_code == CmdCode::LD) {
            if (cmd->getArg(1)->arg_type == ArgType::ARG_DEFAULT) {
              auto src_ref = std::static_pointer_cast<ArgDefault> (cmd->getArg(1))->value();
              auto lbl = DisassemblerCore::inst().makeData(cmd->addr, src_ref, size == ArgSize::Word ? memory::Reference::Type::READ_WORD : memory::Reference::Type::READ_BYTE);
              auto src = std::make_shared<ArgMemoryReference>(src_ref, false);
              src->setLabel(lbl);
              cmd->setArg(1, src);
            }
          }
          if (cmd->command_code != CmdCode::EX || reg != Register16::DE) {
            return;
          }
        }
      } else if (cmd->getArg(0)->arg_type == ArgType::ARG_REGISTER8) {
        auto tst_ref = std::static_pointer_cast<ArgRegister8>(cmd->getArg(0));
        if (reg.isPart(tst_ref->reg_id)) {
          // loading part of register, just break search
          return;
        }
      }
    }
    --tmp_addr;
  } while (tmp_addr.offset() > 0 && isNotBreakCommand(cmd));
}

bool RegisterReferenceScan::isNotBreakCommand(core::CommandPtr cmd) {
  auto cc = cmd->command_code;
  return (cc != CmdCode::CALL) && (cc != CmdCode::DJNZ) && (cc != CmdCode::JP) && (cc != CmdCode::JR) && (cc != CmdCode::RET) && (cc != CmdCode::RETI) && (cc != CmdCode::RETN) && (cc != CmdCode::RST);
}

size_t RegisterReferenceScan::process(CommandPtr cmd, size_t len) {
  int idx = 0;
  switch (cmd->command_code) {
    case CmdCode::LD:
      updateRegisterSource(cmd->addr, cmd->getArg(0), cmd->getArg(1)->getSize());
      updateRegisterSource(cmd->addr, cmd->getArg(1), cmd->getArg(0)->getSize());
      break;
    case CmdCode::AND:
    case CmdCode::CP:
    case CmdCode::DEC:
    case CmdCode::INC:
    case CmdCode::OR:
    case CmdCode::JP:
    case CmdCode::RL:
    case CmdCode::RLC:
    case CmdCode::RR:
    case CmdCode::RRC:
    case CmdCode::SLA:
    case CmdCode::SRA:
    case CmdCode::SRL:
    case CmdCode::SUB:
    case CmdCode::XOR:
      updateRegisterSource(cmd->addr, cmd->getArg(0), ArgSize::Byte);
      break;
    case CmdCode::ADC:
    case CmdCode::ADD:
    case CmdCode::BIT:
    case CmdCode::RES:
    case CmdCode::SBC:
    case CmdCode::SET:
      updateRegisterSource(cmd->addr, cmd->getArg(1), ArgSize::Byte);
      break;
    case CmdCode::CPD:
    case CmdCode::CPDR:
    case CmdCode::CPI:
    case CmdCode::CPIR:
    case CmdCode::IND:
    case CmdCode::INDR:
    case CmdCode::INI:
    case CmdCode::INIR:
    case CmdCode::OUTD:
    case CmdCode::OTDR:
    case CmdCode::OUTI:
    case CmdCode::OTIR:
    case CmdCode::RLD:
    case CmdCode::RRD:
      updateRegisterSource(cmd->addr, Register16::HL, ArgSize::Byte);
      break;
    case CmdCode::LDD:
    case CmdCode::LDDR:
    case CmdCode::LDI:
    case CmdCode::LDIR:
      updateRegisterSource(cmd->addr, Register16::HL, ArgSize::Byte);
      updateRegisterSource(cmd->addr, Register16::DE, ArgSize::Byte);
      break;
  }
  return len;
}

}
}
