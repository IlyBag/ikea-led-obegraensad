#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class SpacePlugin : public Plugin
{
private:
  float issProgress;
  unsigned long lastUpdate;

public:
  static int minutesToPass;
  static bool isVisible;

  static void setISS(int min, bool visible);

  void setup() override;
  void loop() override;
  const char *getName() const override;
};
