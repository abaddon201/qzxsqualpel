#ifndef GUICHUNKLIST_H
#define GUICHUNKLIST_H

#include <memory>
#include <list>

#include "guiblock.h"

namespace dasm {
namespace gui {

class GUIBlockList : public core::MemoryMap<GUIBlockPtr> {
public:
  size_t find_position(size_t pos) {
    return core::MemoryMap<GUIBlockPtr>::find_position([pos](auto& chunk) {
      return ((chunk.elem()->cursorStartPosition() <= pos) && (chunk.elem()->cursorEndPosition() >= pos));
    });
  }
};

}
}
#endif // GUICHUNKLIST_H
