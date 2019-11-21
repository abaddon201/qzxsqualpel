#pragma once

namespace dasm {
namespace core {

enum class JumpType {
  JT_NONE = 0,
  JT_CALL,
  JT_COND_JUMP,
  JT_JUMP,
  JT_COND_RET,
  JT_RET,
};

}
}