#include <QDebug>

#include <algorithm>

#include "guichunk.h"
#include "guichunklist.h"

#include "core/chunk_list.h"

std::shared_ptr<GUIChunk> GUIChunkList::getChunkByPosition(int pos) const {
  auto res = std::find_if(_chunks.begin(), _chunks.end(), [pos](auto chunk) {
    return ((chunk->cursorStartPosition() < pos) && (chunk->cursorEndPosition() >= pos));
  });
  if (res != _chunks.end()) {
    return *res;
  }
  return nullptr;
}

std::shared_ptr<GUIChunk> GUIChunkList::getChunkContains(const Addr &addr) const {
  auto res = std::find_if(_chunks.begin(), _chunks.end(),
                          [&addr](auto chunk) { return (chunk->core()->containsAddr(addr)); });
  if (res != _chunks.end()) {
    return *res;
  }
  return nullptr;
}

void GUIChunkList::update(ChunkList &chunks) {
  _chunks.clear();
  for (auto &ch: chunks.chunks()) {
    _chunks.push_back(std::make_shared<GUIChunk>(ch.second));
  }
}
