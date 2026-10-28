#pragma once

#include "PluginManager.h"

class PacmanPlugin : public Plugin
{
private:
  int pacmanX;
  int ghostX;
  int direction; // 1 = right, -1 = left
  bool poweredUp;
  int mouthFrame;
  bool dots[16];
  unsigned long lastStep;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
