#ifndef MQTT_SEW_HPP
#define MQTT_SEW_HPP
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "lct2ch.h"

class MQTTSew {
  public:
    MQTTSew(WiFiClient &espClient);
    void initMQTT(LCT2CH &relay);
    void handleClient();
    void setConfig(String host, int port, String topic);
    String getInfo();
    bool reconnect();
    bool disconnect();
    void sendPublishOne();
    void sendPublishTwo();
  private:
    PubSubClient *client = NULL;
    String MAC = WiFi.macAddress().c_str();
    String sensorIdOne = MAC + ":00:01";
    String sensorIdTwo = MAC + ":00:02";
    String host = "sew.ovh";
    int port = 30810;
    String topic = "tribeca/";
    String publishOne = sensorIdOne + "/switch/status";
    String publishTwo = sensorIdTwo + "/switch/status";
    String subscribeOne = sensorIdOne + "/switch/action";
    String subscribeTwo = sensorIdTwo + "/switch/action";
    LCT2CH *apiRelay;
    long lastReconnectAttempt = 0;
    void callback(String topic, String payload, unsigned int length);
    void readConfig();
    void saveConfig();
    boolean isConfigChanged = false;
};
#endif
