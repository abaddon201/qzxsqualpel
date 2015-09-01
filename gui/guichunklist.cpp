#include <algorithm>

#include "guichunk.h"
#include "guichunklist.h"

#include "core/CChunkList.h"

std::shared_ptr<GUIChunk> GUIChunkList::getChunkByPosition(int pos) const {
  auto res = std::find_if(_chunks.begin(), _chunks.end(), [pos] (auto chunk) {return ((chunk->cursorStartPosition()<pos) && (chunk->cursorEndPosition()>=pos));});
  if (res != _chunks.end()) {
    return *res;
  }
  return nullptr;
}

std::shared_ptr<GUIChunk> GUIChunkList::getChunkContains(CAddr &addr) const {
  auto res = std::find_if(_chunks.begin(), _chunks.end(), [&addr] (auto chunk) {return (chunk->core()->containsAddr(addr));});
  if (res != _chunks.end()) {
    return *res;
  }
  return nullptr;
}

void GUIChunkList::update(CChunkList &chunks) {
  _chunks.clear();
  for (auto ch: chunks.chunks()) {
    if (ch.second == nullptr)
      break;
    _chunks.push_back(std::make_shared<GUIChunk>(ch.second));
  }
}
