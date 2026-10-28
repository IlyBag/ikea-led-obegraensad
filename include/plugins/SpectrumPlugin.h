#pragma once

#include "PluginManager.h"

class SpectrumPlugin : public Plugin
{
private:
  uint8_t heights[COLS];
  uint8_t targets[COLS];
  float peakY[COLS];
  unsigned long lastUpdate;
  unsigned long lastBeat;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
