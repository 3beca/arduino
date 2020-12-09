#include "parser.hpp"
#include <ESP8266WiFi.h>

String createRelayResponse(String relayId, Status status)
{
    String response = "{\"sensorId\": \"";
    response += WiFi.macAddress().c_str();
    response += ":";
    response += relayId;
    response += "\",";
    response += "\"type\":\"SWITCH\",";
    response += "\"value\":";
    response += status;
    response += "}";
    return response;
}

DynamicJsonDocument parseString(String body, int size)
{
  const size_t capacity = JSON_OBJECT_SIZE(3) + size;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, body);
  return doc;
}
