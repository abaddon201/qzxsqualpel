#pragma once

class IGUIUpdater {
public:
  virtual void updateWidgets() = 0;

  virtual ~IGUIUpdater() = default;
};
