#pragma once

#include "PluginManager.h"

class MatrixRainPlugin : public Plugin
{
private:
  struct Column
  {
    int y;
    int speed;
    int length;
    int nextSpawn;
  };

  Column cols[COLS];
  unsigned long lastUpdate;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
