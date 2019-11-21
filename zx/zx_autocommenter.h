#pragma once

#include "core/autocommenter.h"

#include <string>

class ZXAutoCommenter :public dasm::core::AutoCommenter {
public:
  void commentCommand(dasm::core::Command& out_command) override;

private:
};