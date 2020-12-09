#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "wifi_config.h"
#include "lct2ch.h"
#include "web_server.hpp"

LCT2CH relays;

void setup () 
{
  // Connect to LCT2CH
  relays.connectDevice();

  // Try to connect to last WiFi
  initWifi();

  // Init WebServer + mqtt clinet
  initServer(relays);
  delay(1500);
}

void loop() {
  server.handleClient();
  mqtt.handleClient();
}
