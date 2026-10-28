#include "asyncwebserver.h"
#include "messages.h"
#include "screen.h"
#include "PluginManager.h"
#include "plugins/TransitPlugin.h"
#include "plugins/ClusterPlugin.h"
#include "plugins/RiverPlugin.h"
#include "plugins/AirQualityPlugin.h"
#include "plugins/RainRadarPlugin.h"
#include "plugins/EnergyPlugin.h"
#include "plugins/SpacePlugin.h"
#include <ArduinoJson.h>

#ifdef ENABLE_SERVER

AsyncWebServer server(80);

void initWebServer()
{
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Accept, Content-Type, Authorization");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

  server.on("/", HTTP_GET, startGui);
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(404, "text/plain", "Page not found!"); });

  // Route to handle http://your-server/message?text=Hello&repeat=3&id=42&delay=30
  server.on("/message", HTTP_GET, handleMessage);
  server.on("/removemessage", HTTP_GET, handleRemove);

  // --- REST API for Home Assistant & Automation ---
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    DynamicJsonDocument doc(2048);
    doc["brightness"] = Screen.getCurrentBrightness();
    doc["rotation"] = Screen.currentRotation;
    doc["ticker_active"] = Messages.hasContinuous();
    Plugin *active = pluginManager.getActivePlugin();
    doc["active_id"] = active ? active->getId() : 1;
    doc["active_name"] = active ? active->getName() : "None";
    JsonArray pluginsArr = doc.createNestedArray("plugins");
    for (Plugin *p : pluginManager.getAllPlugins())
    {
      JsonObject pObj = pluginsArr.createNestedObject();
      pObj["id"] = p->getId();
      pObj["name"] = p->getName();
    }
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.on("/api/plugin", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("id"))
    {
      int id = request->getParam("id")->value().toInt();
      pluginManager.setActivePluginById(id);
      if (request->hasParam("save"))
      {
        pluginManager.persistActivePlugin();
      }
      request->send(200, "application/json", "{\"status\":\"ok\",\"active_id\":" + String(id) + "}");
    }
    else if (request->hasParam("name"))
    {
      String name = request->getParam("name")->value();
      pluginManager.setActivePlugin(name.c_str());
      if (request->hasParam("save"))
      {
        pluginManager.persistActivePlugin();
      }
      request->send(200, "application/json", "{\"status\":\"ok\",\"active_name\":\"" + name + "\"}");
    }
    else
    {
      request->send(400, "application/json", "{\"error\":\"Missing id or name parameter\"}");
    }
  });

  server.on("/api/brightness", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("value"))
    {
      int val = request->getParam("value")->value().toInt();
      if (val < 0) val = 0;
      if (val > 255) val = 255;
      Screen.setBrightness((uint8_t)val);
      request->send(200, "application/json", "{\"status\":\"ok\",\"brightness\":" + String(val) + "}");
    }
    else
    {
      request->send(400, "application/json", "{\"error\":\"Missing value parameter (0-255)\"}");
    }
  });

  // Rotation control: ?value=0|1|2|3 or ?deg=0|90|180|270
  server.on("/api/rotation", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("value"))
    {
      int val = request->getParam("value")->value().toInt();
      Screen.setRotation(val);
      request->send(200, "application/json", "{\"status\":\"ok\",\"rotation\":" + String(Screen.currentRotation) + "}");
    }
    else if (request->hasParam("deg"))
    {
      int deg = request->getParam("deg")->value().toInt();
      int rot = (deg / 90) % 4;
      Screen.setRotation(rot);
      request->send(200, "application/json", "{\"status\":\"ok\",\"rotation\":" + String(Screen.currentRotation) + ",\"deg\":" + String(rot * 90) + "}");
    }
    else
    {
      request->send(200, "application/json", "{\"rotation\":" + String(Screen.currentRotation) + ",\"deg\":" + String(Screen.currentRotation * 90) + "}");
    }
  });

  // Ticker stop endpoint
  server.on("/api/ticker/stop", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Messages.stopContinuous();
    request->send(200, "application/json", "{\"status\":\"ok\",\"message\":\"Ticker stopped\"}");
  });

  // Transit (ÖPNV) endpoint: /api/transit?line1=U1&min1=3&line2=BUS12&min2=8&activate=1
  server.on("/api/transit", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String l1 = request->hasParam("line1") ? request->getParam("line1")->value() : (request->hasParam("line") ? request->getParam("line")->value() : TransitPlugin::line1);
    int m1 = request->hasParam("min1") ? request->getParam("min1")->value().toInt() : (request->hasParam("min") ? request->getParam("min")->value().toInt() : TransitPlugin::min1);
    String l2 = request->hasParam("line2") ? request->getParam("line2")->value() : TransitPlugin::line2;
    int m2 = request->hasParam("min2") ? request->getParam("min2")->value().toInt() : TransitPlugin::min2;

    TransitPlugin::setTransit(l1, m1, l2, m2);

    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("Transit");
    }

    DynamicJsonDocument doc(512);
    doc["status"] = "ok";
    doc["line1"] = TransitPlugin::line1;
    doc["min1"] = TransitPlugin::min1;
    doc["line2"] = TransitPlugin::line2;
    doc["min2"] = TransitPlugin::min2;
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });

  // Cluster Node & Incident endpoint: /api/cluster?node=runner&load=35&online=1&threats=0&activate=1
  server.on("/api/cluster", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("node"))
    {
      String nStr = request->getParam("node")->value();
      int nodeIdx = -1;
      if (nStr == "0" || nStr == "gw" || nStr == "securegw") nodeIdx = 0;
      else if (nStr == "1" || nStr == "plx" || nStr == "plantlix") nodeIdx = 1;
      else if (nStr == "2" || nStr == "run" || nStr == "runner" || nStr == "vps-runner") nodeIdx = 2;
      else if (nStr == "3" || nStr == "sto" || nStr == "storage" || nStr == "storage-vps") nodeIdx = 3;
      else if (nStr == "4" || nStr == "ha" || nStr == "ha-core") nodeIdx = 4;
      else nodeIdx = nStr.toInt();

      int load = request->hasParam("load") ? request->getParam("load")->value().toInt() : 50;
      bool online = request->hasParam("online") ? (request->getParam("online")->value().toInt() != 0) : true;

      ClusterPlugin::setNode(nodeIdx, load, online);
    }

    if (request->hasParam("threats"))
    {
      ClusterPlugin::setThreats(request->getParam("threats")->value().toInt());
    }

    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("Cluster");
    }

    DynamicJsonDocument doc(1024);
    doc["status"] = "ok";
    doc["threat_count"] = ClusterPlugin::threatCount;
    JsonArray nodesArr = doc.createNestedArray("nodes");
    for (int i = 0; i < 5; i++)
    {
      JsonObject no = nodesArr.createNestedObject();
      no["name"] = ClusterPlugin::nodes[i].name;
      no["load"] = ClusterPlugin::nodes[i].load;
      no["online"] = ClusterPlugin::nodes[i].online;
    }
    String resp;
    serializeJson(doc, resp);
    request->send(200, "application/json", resp);
  });

  // Security Alert endpoint: /api/alert?title=SSH+Attack&source=Sentinel
  server.on("/api/alert", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String text = request->hasParam("text") ? request->getParam("text")->value() : 
                 (request->hasParam("title") ? request->getParam("title")->value() : "SECURITY ALERT");
    
    if (request->hasParam("source"))
    {
      text = "[" + request->getParam("source")->value() + "] " + text;
    }

    ClusterPlugin::triggerAlert(text);
    request->send(200, "application/json", "{\"status\":\"ok\",\"alert\":\"" + text + "\"}");
  });

  // River Pegelstand endpoint: /api/river?cm=142&trend=1&activate=1
  server.on("/api/river", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int cm = request->hasParam("cm") ? request->getParam("cm")->value().toInt() : RiverPlugin::levelCm;
    int trend = request->hasParam("trend") ? request->getParam("trend")->value().toInt() : RiverPlugin::trend;
    RiverPlugin::setRiver(cm, trend);
    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("River");
    }
    request->send(200, "application/json", "{\"status\":\"ok\",\"cm\":" + String(RiverPlugin::levelCm) + ",\"trend\":" + String(RiverPlugin::trend) + "}");
  });

  // Air Quality AQI endpoint: /api/air?aqi=28&activate=1
  server.on("/api/air", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int aqi = request->hasParam("aqi") ? request->getParam("aqi")->value().toInt() : AirQualityPlugin::aqi;
    AirQualityPlugin::setAQI(aqi);
    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("Air Quality");
    }
    request->send(200, "application/json", "{\"status\":\"ok\",\"aqi\":" + String(AirQualityPlugin::aqi) + "}");
  });

  // Rain Radar endpoint: /api/rain?min=15&intensity=65&activate=1
  server.on("/api/rain", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int min = request->hasParam("min") ? request->getParam("min")->value().toInt() : RainRadarPlugin::minutesToRain;
    int intensity = request->hasParam("intensity") ? request->getParam("intensity")->value().toInt() : RainRadarPlugin::rainIntensity;
    RainRadarPlugin::setRain(min, intensity);
    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("Rain Radar");
    }
    request->send(200, "application/json", "{\"status\":\"ok\",\"min\":" + String(RainRadarPlugin::minutesToRain) + ",\"intensity\":" + String(RainRadarPlugin::rainIntensity) + "}");
  });

  // Energy Grid endpoint: /api/energy?percent=78&activate=1
  server.on("/api/energy", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int pct = request->hasParam("percent") ? request->getParam("percent")->value().toInt() : EnergyPlugin::greenPercent;
    EnergyPlugin::setGreen(pct);
    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("Energy Grid");
    }
    request->send(200, "application/json", "{\"status\":\"ok\",\"green_percent\":" + String(EnergyPlugin::greenPercent) + "}");
  });

  // Space / ISS Tracker endpoint: /api/iss?min=3&visible=1&activate=1
  server.on("/api/iss", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int min = request->hasParam("min") ? request->getParam("min")->value().toInt() : SpacePlugin::minutesToPass;
    bool vis = request->hasParam("visible") ? (request->getParam("visible")->value().toInt() != 0) : SpacePlugin::isVisible;
    SpacePlugin::setISS(min, vis);
    if (request->hasParam("activate"))
    {
      pluginManager.setActivePlugin("ISS Tracker");
    }
    request->send(200, "application/json", "{\"status\":\"ok\",\"min_to_pass\":" + String(SpacePlugin::minutesToPass) + ",\"visible\":" + String(SpacePlugin::isVisible ? "true" : "false") + "}");
  });

  server.on("/api/sensors", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    DynamicJsonDocument doc(1024);
#ifdef ESP32
    float tempC = temperatureRead();
    float tempF = tempC * 1.8f + 32.0f;
    doc["temp_c"] = (int)(tempC * 10.0f) / 10.0f;
    doc["temp_f"] = (int)(tempF * 10.0f) / 10.0f;
    doc["hall"] = hallRead();
    doc["cpu_mhz"] = ESP.getCpuFreqMHz();
#else
    doc["temp_c"] = 0;
    doc["temp_f"] = 0;
    doc["hall"] = 0;
    doc["cpu_mhz"] = 80;
#endif
    doc["free_heap"] = ESP.getFreeHeap();
    doc["total_heap"] = ESP.getHeapSize();
    doc["flash_size"] = ESP.getFlashChipSize();
    doc["sketch_size"] = ESP.getSketchSize();
    doc["free_sketch"] = ESP.getFreeSketchSpace();
    doc["rssi"] = WiFi.RSSI();
    doc["uptime"] = millis() / 1000;
    doc["ip"] = WiFi.localIP().toString();
    doc["mac"] = WiFi.macAddress();
    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  server.begin();
}

#endif
