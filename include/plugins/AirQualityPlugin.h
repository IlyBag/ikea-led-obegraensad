#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class AirQualityPlugin : public Plugin
{
private:
  float breathPhase;
  unsigned long lastUpdate;
  struct Particle {
    float x;
    float y;
    float vx;
  } particles[8];

public:
  static int aqi;

  static void setAQI(int value);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
