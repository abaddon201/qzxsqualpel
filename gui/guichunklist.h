#ifndef GUICHUNKLIST_H
#define GUICHUNKLIST_H

#include <memory>
#include <list>

#include "guichunk.h"
#include "memory/addr.h"
#include "core/chunk_list.h"

template<typename T>
class GUIBlockList {
public:
  using GUIBlockPtr = std::shared_ptr<GUIBlock<T>>;
  GUIBlockPtr getChunkByPosition(int pos) const {
    auto res = std::find_if(_chunks.begin(), _chunks.end(), [pos](auto chunk) {
      return ((chunk->cursorStartPosition() < pos) && (chunk->cursorEndPosition() >= pos));
    });
    if (res != _chunks.end()) {
      return *res;
    }
    return nullptr;
  }

  GUIBlockPtr getChunkContains(const dasm::memory::Addr& addr) const {
    auto res = std::find_if(_chunks.begin(), _chunks.end(),
                            [&addr](auto chunk) { return (chunk->core()->containsAddr(addr)); });
    if (res != _chunks.end()) {
      return *res;
    }
    return nullptr;
  }

  std::list<GUIBlockPtr>& chunks() { return _chunks; }

  void clear() {
    _chunks.clear();
  }

  void push(GUIBlockPtr elem) {
    _chunks.push_back(elem);
  }
  void update( std::vector<T>& chunks) {
    for (auto& ch : chunks) {
      auto ptr = std::make_shared<GUIBlock<T>>(std::shared_ptr<T>(&ch));
      _chunks.push_back(ptr);
    }
  }

  template<typename S>
  void update(std::map<S, std::shared_ptr<T>>& chunks) {
    for (auto& ch : chunks) {
      _chunks.push_back(std::make_shared<GUIBlock<T>>(ch.second));
    }
  }
private:
  std::list<GUIBlockPtr> _chunks;
};

#endif // GUICHUNKLIST_H
