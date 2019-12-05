#ifndef GUI_TEXT_BLOCK_USER_DATA_H
#define GUI_TEXT_BLOCK_USER_DATA_H

#include <QTextBlockUserData>
#include <memory>

#include "core/chunk.h"

struct GUITextBlockUserData : public QTextBlockUserData {
  GUITextBlockUserData(std::shared_ptr<dasm::core::Chunk> ch, dasm::core::CommandPtr cm) : chunk{ch}, cmd{cm} {}

  std::shared_ptr<dasm::core::Chunk> chunk;
  dasm::core::CommandPtr cmd;
};

#endif // GUI_TEXT_BLOCK_USER_DATA_H

