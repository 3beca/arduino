#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP
#include <ArduinoJson.h>
#include "wifi_config.h"
#include "lct2ch.h"
#include "mqtt_sew.hpp"
#include "parser.hpp"

ESP8266WebServer server(80);
WiFiClient espClient;
MQTTSew mqtt(espClient);
LCT2CH *RelayApi = NULL;

DynamicJsonDocument parseBody(int size)
{
  return parseString(server.arg("plain"), size);
}

void handleModeAp() 
{
      WiFiMode_t mode = WiFi.getMode();
      if (mode != WIFI_AP)
      {
        if (server.method() != HTTP_POST)
        {
          server.send(200, "application/json", getConfiguration());
        }
        else
        {
          server.send(200, "application/json", getConfigurationAP());
          delay(500);
          enableWifiAP();
        }
      }
      else
      {
        server.send(200, "application/json", getConfiguration());
      }
}

 void handleModeSta() 
{
  WiFiMode_t mode = WiFi.getMode();
  if (mode != WIFI_STA)
  {
    if (server.method() != HTTP_POST)
    {
      server.send(200, "application/json", getConfiguration());
    }
    else
    {
      DynamicJsonDocument doc = parseBody(60);
      String ssid = doc["ssid"]; // "CenticWifi"
      String passwd = doc["pwd"]; // "123456789"
      if(ssid.length() > 3)
      {
        // Try to connect firsst!
        bool tryNewConnection = tryWifiClient(ssid.c_str(), passwd.c_str());
        // delay(3000);
        if(tryNewConnection)
        {
          server.send(200, "application/json", getConfiguration());
          delay(3000);
          enableWifiClient(ssid.c_str(), passwd.c_str());
        }
        else
        {
          server.send(200, "application/json", getConfigurationAP());
        }
      }
      else
      {
        server.send(400, "application/json", "{\"code\": 404, \"error\": \"Invalid SSID, min 3 chars\"}");
      }
    }
  }
  else
  {
    server.send(200, "application/json", getConfiguration());
  }
}

 void handleInfo() 
{
    server.send(200, "application/json", getConfiguration());
}

void handleRelay()
{
  if (server.method() != HTTP_POST)
  {
    Status statusOne = RelayApi->getRelayStatus(Relay::ONE);
    Status statusTwo = RelayApi->getRelayStatus(Relay::TWO);
    String response = "[" + createRelayResponse("00:01", statusOne) + "," + createRelayResponse("00:02", statusTwo) + "]";
    server.send(200, "application/json", response);
  }
  else
  {
    DynamicJsonDocument doc = parseBody(60);
    String id = doc["id"]; // "F4:CF:A2:E3:BC:ED:00:01"
    String type = doc["type"]; // "SWITCH"
    int value = doc["value"]; // 1
    
    Status status;
    if(id.endsWith("00:01"))
    {
      status = RelayApi->setRelayStatus(Relay::ONE, value == 1 ? Status::ON : Status::OFF);
      server.send(200, "application/json", createRelayResponse("00:01", status));
    }
    else if(id.endsWith("00:02"))
    {
      status = RelayApi->setRelayStatus(Relay::TWO, value == 1 ? Status::ON : Status::OFF);
      server.send(200, "application/json", createRelayResponse("00:02", status));
    }
    else
    {
      server.send(404, "application/json", "{\"code\": 404, \"error\": \"Relay " + id + " not found\"}"); 
    }
  }
}

void handleMQTT() 
{
      WiFiMode_t mode = WiFi.getMode();
      if (mode != WIFI_AP)
      {
        if (server.method() != HTTP_POST)
        {
          server.send(200, "application/json", mqtt.getInfo());
        }
        else
        {
          DynamicJsonDocument doc = parseBody(256);
          String host = doc["host"]; // "sew.tribeca.ovh"
          int port = doc["port"]; // 1885
          String topic = doc["topic"]; // juanjo/casa1

          mqtt.disconnect();
          mqtt.setConfig(host, port, topic);
          bool connected = mqtt.reconnect();
          if(connected)
          {
            server.send(200, "application/json", mqtt.getInfo());
          }
          else {
            server.send(400, "application/json", "{\"code\": 400, \"details\":\"Cannot connect to MQTT\"}");
          }
        }
      }
      else
      {
        server.send(200, "application/json", getConfiguration());
      }
}

void handle404() {
  server.send(404, "application/json", "{\"code\": 404, \"error\": \"Endpoint not found\"}");
}


void initServer(LCT2CH &relay)
{
    // Add Relay to Webserver
    RelayApi = &relay;
    // Rutas'
    server.on("/mode/ap", handleModeAp);
    server.on("/mode/sta", handleModeSta);
    server.on("/info", handleInfo);
    server.on("/relay", handleRelay);
    server.on("/mqtt", handleMQTT);
    server.onNotFound(handle404);
    
    // Iniciar servidor
    server.begin();
    mqtt.initMQTT(relay);
    mqtt.reconnect();
}
#endif
