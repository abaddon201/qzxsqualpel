#ifndef GUI_TEXT_BLOCK_USER_DATA_H
#define GUI_TEXT_BLOCK_USER_DATA_H

#include <QTextBlockUserData>
#include <memory>

#include "core/chunk.h"

struct GUITextBlockUserData : public QTextBlockUserData {
  GUITextBlockUserData(std::shared_ptr<Chunk> ch, Command *cm) : chunk{ch}, cmd{cm} {}

  std::shared_ptr<Chunk> chunk;
  Command *cmd;
};

#endif // GUI_TEXT_BLOCK_USER_DATA_H

