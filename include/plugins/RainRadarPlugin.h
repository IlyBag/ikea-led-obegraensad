#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class RainRadarPlugin : public Plugin
{
private:
  unsigned long lastUpdate;
  int dropY[5];
  int dropX[5];

public:
  static int minutesToRain;
  static int rainIntensity;

  static void setRain(int minutes, int intensity);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
