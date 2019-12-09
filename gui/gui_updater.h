#pragma once

class IGUIUpdater {
public:
  virtual void updateWidgets() = 0;

  virtual void onAddressUpdated(uint16_t addr, uint16_t bytes) = 0;

  virtual void onPositionChanged(int pos) = 0;

  virtual ~IGUIUpdater() = default;
};
