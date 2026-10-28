#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class EnergyPlugin : public Plugin
{
private:
  unsigned long lastRotate;
  int rotorFrame;

public:
  static int greenPercent;

  static void setGreen(int percent);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
