#pragma once

#include "PluginManager.h"
#include <Arduino.h>

class SensorsOverviewPlugin : public Plugin
{
private:
  unsigned long lastUpdate;
  int animFrame;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};

class ChipTempPlugin : public Plugin
{
private:
  unsigned long lastUpdate;
  int flamePhase;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};

class HallSensorPlugin : public Plugin
{
private:
  unsigned long lastUpdate;
  int pulsePhase;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};

class WifiSignalPlugin : public Plugin
{
private:
  unsigned long lastUpdate;
  int waveStep;

public:
  void setup() override;
  void loop() override;
  const char *getName() const override;
};
