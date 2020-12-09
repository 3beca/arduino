#ifndef PARSER_HPP
#define PARSER_HPP
#include <ArduinoJson.h>
#include "lct_data.h"
String createRelayResponse(String relayId, Status status);
DynamicJsonDocument parseString(String body, int size);
#endif
