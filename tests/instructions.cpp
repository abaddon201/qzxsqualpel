#include "catch.hpp"
#include "core/disassembler_core.h"

class GU : public IGUIUpdater {
public:
  void updateWidgets() override {};
  void onPositionChanged(int pos) override {};
  void onAddressUpdated(uint16_t addr, uint16_t bytes) override {};
};

TEST_CASE("disassembler", "[core]") {
  dasm::utils::initHexes();
  auto fileName = "../../../../xeno2.raw";
  auto& core = dasm::core::DisassemblerCore::inst();
  core.init(new GU());
  auto* buf = new unsigned char[65536];
  size_t loaded;
  FILE* f = fopen(fileName, "rb");
  loaded = fread(buf, 1, 65536, f);
  fclose(f);
  core.setRawMemory(buf, loaded);
  core.setFileName(fileName);
  delete[] buf;
  ///@fixme: Add checks

  dasm::core::DisassemblerCore::inst().initialParse();
  uint16_t ret_addr = 0;
  dasm::core::DisassemblerCore::inst().disassembleBlock(ret_addr);
  ret_addr = 0xcd78;
  dasm::core::DisassemblerCore::inst().disassembleBlock(ret_addr);
  
  SECTION("LD") {
    auto cmd = core.commands().get(0x03c1);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "IX");
    REQUIRE(cmd->getArg(1)->toString() == "03d1");

    cmd = core.commands().get(0x0c10);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "DE");
    REQUIRE(cmd->getArg(1)->toString() == "0095");

    cmd = core.commands().get(0x0008);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "(word_5c5d)");

    cmd = core.commands().get(0x000b);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "(word_5c5f)");
    REQUIRE(cmd->getArg(1)->toString() == "HL");

    cmd = core.commands().get(0x0c90);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "(byte_5c8c)");
    REQUIRE(cmd->getArg(1)->toString() == "A");

    cmd = core.commands().get(0x03d9);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "C");
    REQUIRE(cmd->getArg(1)->toString() == "3f");

    cmd = core.commands().get(0x0b9b);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "A");
    REQUIRE(cmd->getArg(1)->toString() == "(byte_5c91)");

    cmd = core.commands().get(0x0c0b);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "H");
    REQUIRE(cmd->getArg(1)->toString() == "00");

    cmd = core.commands().get(0x12a2);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "(IY+31)");
    REQUIRE(cmd->getArg(1)->toString() == "02");

    //0x1222 LD HL, (word_5cb2)
    cmd = core.commands().get(0x1222);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "(word_5cb2)");

    //0x1225 LD (HL), 3e
    cmd = core.commands().get(0x1225);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "(HL)");
    REQUIRE(cmd->getArg(1)->toString() == "3e");

    //0x137c LD HL,5C44
    cmd = core.commands().get(0x137c);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "byte_5c44");

    //0x0daf LD HL,0000
    cmd = core.commands().get(0x0daf);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "0000");

    //0x0dc9 LD HL, (word_5c51) //wrong label (not inited) 0x1615 - address of first initialization
    cmd = core.commands().get(0x0dc9);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "(word_5c51)");

    //0x0edf LD HL, byte_5b00
    cmd = core.commands().get(0x0edf);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "byte_5b00");

    //0x0ee7 LD (HL), A
    cmd = core.commands().get(0x0ee7);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "(HL)");
    REQUIRE(cmd->getArg(1)->toString() == "A");
  }
  SECTION("Logic ops") {
    //0x0bdf AND 03
    auto cmd = core.commands().get(0x0bdf);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::AND);
    REQUIRE(cmd->getArg(0)->toString() == "03");
    REQUIRE(cmd->getArgsCount() == 1);

    //0x0be1 OR 58
    cmd = core.commands().get(0x0be1);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::OR);
    REQUIRE(cmd->getArg(0)->toString() == "58");
    REQUIRE(cmd->getArgsCount() == 1);
  }

  SECTION("Bit ops") {
    //0x2aa9 res 6,(IY+1)
    auto cmd = core.commands().get(0x2aa9);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::RES);
    REQUIRE(cmd->getArg(0)->toString() == "6");
    REQUIRE(cmd->getArg(1)->toString() == "(IY+01)");

    //0x1287 set 1, (IY+1)
    cmd = core.commands().get(0x1287);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::SET);
    REQUIRE(cmd->getArg(0)->toString() == "1");
    REQUIRE(cmd->getArg(1)->toString() == "(IY+01)");

    //0x137f BIT 7, (HL)
    cmd = core.commands().get(0x137f);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::BIT);
    REQUIRE(cmd->getArg(0)->toString() == "7");
    REQUIRE(cmd->getArg(1)->toString() == "(HL)");

  }

  SECTION("Arith ops") {
    //0x0b52 SUB a5
    auto cmd = core.commands().get(0x0b52);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::SUB);
    REQUIRE(cmd->getArg(0)->toString() == "a5");
    REQUIRE(cmd->getArgsCount() == 1);

    //0x0b56 ADD A,15
    cmd = core.commands().get(0x0b56);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::ADD);
    REQUIRE(cmd->getArg(0)->toString() == "A");
    REQUIRE(cmd->getArg(1)->toString() == "15");

    //0x007d CP 21
    cmd = core.commands().get(0x007d);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::CP);
    REQUIRE(cmd->getArg(0)->toString() == "21");
    REQUIRE(cmd->getArgsCount() == 1);
  }

  SECTION("Port, iterupts, regs ops") {
    //0x11ce OUT (fe), A
    auto cmd = core.commands().get(0x11ce);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::OUT);
    REQUIRE(cmd->getArg(0)->toString() == "(fe)");
    REQUIRE(cmd->getArg(1)->toString() == "A");

    //0x122e IM 1
    cmd = core.commands().get(0x122e);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::IM);
    REQUIRE(cmd->getArg(0)->toString() == "1");
    REQUIRE(cmd->getArgsCount() == 1);

    //0x0bbd EX AF, AF'
    cmd = core.commands().get(0x0bbd);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::EX);
    auto v = cmd->getArg(1)->toString();
    REQUIRE(cmd->getArg(0)->toString() == "AF");
    REQUIRE(cmd->getArg(1)->toString() == "AF'");
  }

  SECTION("jmp, calls ops") {
    //0x2e24 (rst 28)
    auto cmd = core.commands().get(0x2e24);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::RST);
    REQUIRE(cmd->getArg(0)->toString() == "28");
    REQUIRE(cmd->getArgsCount() == 1);

    //0x0cf4 DJNZ 0x0cf0
    cmd = core.commands().get(0x0cf4);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::DJNZ);
    REQUIRE(cmd->getArg(0)->toString() == "jmp_0cf0");
    REQUIRE(cmd->getArgsCount() == 1);
  }

  SECTION("block ops") {
    //0x120a LDDR
    auto cmd = core.commands().get(0x120a);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LDDR);
    REQUIRE(cmd->getArgsCount() == 0);
  }

  //SECTION("data") {
    //0x5c72 DW ffff
    //auto cmd = core.commands().get(0x5c72);
    //REQUIRE(cmd->command_code == dasm::core::CmdCode::DW);
    //REQUIRE(cmd->getArg(0)->toString() == "ffff");
    //REQUIRE(cmd->getArgsCount() == 1);
  //}

  SECTION("decode") {
    //d50f      20 ff              JR   NZ, d510 (addr = d50f + 2 +(-1))
    auto cmd = core.commands().get(0xd50f);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::JR);
    REQUIRE(cmd->getArg(0)->toString() == "NZ");
    REQUIRE(cmd->getArg(1)->toString() == "d510");

    //d511      21 23 48           LD   HL, 4823
    cmd = core.commands().get(0xd511);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "HL");
    REQUIRE(cmd->getArg(1)->toString() == "4823");

    //d514      16 59              LD   D, 59
    cmd = core.commands().get(0xd514);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::LD);
    REQUIRE(cmd->getArg(0)->toString() == "D");
    REQUIRE(cmd->getArg(1)->toString() == "59");

    //d516      3a                 db   3a        ;:
    cmd = core.commands().get(0xd516);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::NONE);
    REQUIRE(cmd->getArg(0)->toString() == "3a");
    REQUIRE(cmd->getArgsCount() == 1);

    //d517      87                 ADD  A, A
    cmd = core.commands().get(0xd517);
    REQUIRE(cmd->command_code == dasm::core::CmdCode::ADD);
    REQUIRE(cmd->getArg(0)->toString() == "A");
    REQUIRE(cmd->getArg(1)->toString() == "A");

  }

  /*
  ABBY
0x11a7 (back jump)
0x1276 (index reg) (navigation bug)
*/

}
