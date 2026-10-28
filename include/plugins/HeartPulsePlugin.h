#pragma once

#include "PluginManager.h"

class HeartPulsePlugin : public Plugin
{
private:
  unsigned long startTime;
  unsigned long lastFrame;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
