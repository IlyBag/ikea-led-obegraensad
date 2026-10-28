#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class RiverPlugin : public Plugin
{
private:
  float wavePhase;
  unsigned long lastUpdate;

public:
  static int levelCm;
  static int trend; // -1 = fallend, 0 = gleich, 1 = steigend

  static void setRiver(int cm, int t);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
