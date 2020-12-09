#include <EEPROM.h>
#include "mqtt_sew.hpp"
#include "parser.hpp"

void MQTTSew::callback(String topic, String payload, unsigned int length)
{  
  DynamicJsonDocument doc = parseString(payload, 60);
  String sensorId = doc["sensorId"]; // "F4:CF:A2:E3:BC:ED::00:01"
  int value = doc["value"]; // 0 / 1
  String type = doc["type"]; // "SWITCH"
  
  if(type == "SWITCH")
  {
    if (sensorId == sensorIdOne)
    {
      if(value == 0 || value == 1)
      {
        apiRelay->setRelayStatus(Relay::ONE, value == 1 ? Status::ON : Status::OFF);
      }
      sendPublishOne();
    }
    else if (sensorId == sensorIdTwo)
    {
      if(value == 0 || value == 1)
      {
        apiRelay->setRelayStatus(Relay::TWO, value == 1 ? Status::ON : Status::OFF);
      }
      sendPublishTwo();
    }
  }
}

void MQTTSew::sendPublishOne()
{
  String topicOne = topic + publishOne;
  client->publish(topicOne.c_str(), createRelayResponse("00:01", apiRelay->getRelayStatus(Relay::ONE)).c_str());
}

void MQTTSew::sendPublishTwo()
{
  String topicTwo = topic + publishTwo;
  client->publish(topicTwo.c_str(), createRelayResponse("00:02", apiRelay->getRelayStatus(Relay::TWO)).c_str());
}

void MQTTSew::readConfig()
{
  String creds;
  EEPROM.begin(512);
  for(int i = 0; i < 512; i++)
  {
    byte ch = EEPROM.read(i);
    creds += (char)ch;
    if(ch == '\0') {
      break;
    }
  }
  
  DynamicJsonDocument doc = parseString(creds, 512);
  String host = doc["host"];
  int port = doc["port"];
  String topic = doc["topic"];
  if(host != "null") this->host = host;
  if(port != 0) this->port = port;
  if(topic != "null") this->topic = topic;
}

void MQTTSew::saveConfig()
{
  String creds = "{\"host\":\"" + this->host + "\", \"port\": " + this->port + ", \"topic\":\"" + this->topic + "\"}";
  EEPROM.begin(512);
  for(int i = 0; i < creds.length(); i++)
  {
    EEPROM.write(i, creds[i]);
  }
  EEPROM.write(creds.length(), '\0');
  EEPROM.commit();
}

MQTTSew::MQTTSew(WiFiClient &espClient)
{
  client = new PubSubClient(espClient);
}

void MQTTSew::initMQTT(LCT2CH &relay)
{
  apiRelay = &relay;
  readConfig();
  client->setServer(host.c_str(), port);
  client->setCallback (
    [this] (char* topic, byte* payload, unsigned int length) {
      String strTopic = topic;
      char msg[length + 1];
      for(int i = 0; i < length; i++)
      {
        msg[i] = (char)payload[i];
      }
      msg[length] = '\0';
      String strPayload = String(msg);
      this->callback(strTopic, strPayload, length);
    }
  );
}

void MQTTSew::handleClient()
{
  if (!client->connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    // Client connected
    client->loop();
  }
}

void MQTTSew::setConfig(String host, int port, String topic)
{
  this->host = host;
  this->port = port;
  this->topic = topic;
  isConfigChanged = true;
  client->setServer(this->host.c_str(), this->port);
}

String MQTTSew::getInfo()
{
  String response = "{\"host\":\"";
  response += host;
  response += "\",\"port\":";
  response += String(port);
  response += ",\"topic\":\"";
  response += topic;

  response += "\",\"publish\":[\"";
  response += topic + publishOne;
  response += "\",\"";
  response += topic + publishTwo;
  response += "\"]";

  response += ",\"subscribe\":[\"";
  response += topic + subscribeOne;
  response += "\",\"";
  response += topic + subscribeTwo;
  response += "\"]";
  
  response += ",\"connected\":";
  response += client->connected() ? "true" : "false";
  response += ",\"clientId\":\"";
  response += WiFi.macAddress().c_str();
  response += "\"}";

  return response;
}

bool MQTTSew::reconnect()
{
  if (client->connect(WiFi.macAddress().c_str())) {
    // resuscribimos cada vez q se desconecta
    String topicOne = topic + subscribeOne;
    client->subscribe(topicOne.c_str());
    String topicTwo = topic + subscribeTwo;
    client->subscribe(topicTwo.c_str());
    // Cada vez que reconectamos publicamos el estado actual
    sendPublishOne();
    delay(500);
    sendPublishTwo();
    if(isConfigChanged)
    {
      isConfigChanged = false;
      saveConfig();
    }
  }
  return client->connected();
}

bool MQTTSew::disconnect()
{
  if(client != NULL) {
    client->disconnect();
    return true; 
  }
  return false;
}
