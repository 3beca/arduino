#include <ESP8266WiFi.h>
#include "wifi_config.h";

static const char* default_tribeca_ssid = "Tribeca";
static const char* default_tribeca_password = "123456789";

String getConfigurationAP()
{
  
  String response = "{";
  response += "\"ssid\":\"";
  response += "Tribeca";
  response += "\",";

  response += "\"password\":\"";
  response += "123456789";
  response += "\",";

  response += "\"mode\":\"AP\",";

  response += "\"ip\":\"";
  response += "192.168.4.1";
  response += "\","; 

  response += "\"mac\":\"";
  response += WiFi.macAddress().c_str();
  response += "\",";

  response += "\"rssi\":";
  response += WiFi.RSSI();
  response += "}";

  return response;
}

String getConfiguration()
{
  WiFiMode_t mode = WiFi.getMode();
  
  String response = "{";
  response += "\"ssid\":\"";
  response += mode == WIFI_AP ? "Tribeca" : WiFi.SSID().c_str();
  response += "\",";

  response += "\"mode\":\"";
  response += mode == WIFI_AP ? "AP" : (mode == WIFI_STA ? "STA" : "AP+STA");
  response += "\",";

  if (mode == WIFI_AP)
  {
    response += "\"ipAP\":\"";
    response += WiFi.softAPIP().toString();
    response += "\",";  
  }
  else if(mode == WIFI_AP_STA)
  {
    response += "\"ipAP\":\"";
    response += WiFi.softAPIP().toString();
    response += "\",";

    response += "\"ipClient\":\"";
    response += WiFi.localIP().toString();
    response += "\",";
  }
  else
  {
    response += "\"ipClient\":\"";
    response += WiFi.localIP().toString();
    response += "\","; 
  }

  response += "\"gateway\":\"";
  response += WiFi.gatewayIP().toString();
  response += "\",";

  response += "\"dns\":\"";
  response += WiFi.dnsIP().toString();
  response += "\",";

  response += "\"mac\":\"";
  response += WiFi.macAddress().c_str();
  response += "\",";

  response += "\"rssi\":";
  response += WiFi.RSSI();
  response += "}";

  return response;
}

void initWifi()
{
  // Disconnect from AP
  WiFiMode_t mode = WiFi.getMode();
  if (mode != WIFI_STA)
  {
    while(!WiFi.softAPdisconnect());  
  }
  // Connect with client
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  bool tryToConnect = true;
  int tries = 0;
  while(tryToConnect)
  {
    int status = WiFi.status();
    if (status == WL_CONNECTED)
    {
      getConfiguration();
      tryToConnect = false;
    }
    tries++;
    delay(500);
    if (tries > 20) {
      tryToConnect = false;
      enableWifiAP();
    }
  }
}

void enableWifiAP()
{
   WiFi.disconnect(true);
   WiFi.mode(WIFI_AP_STA);
   delay(100);
   while(!WiFi.softAP(default_tribeca_ssid, default_tribeca_password))
   {
     delay(100);
   }
   getConfiguration();
}

void enableWifiClient(const char* ssid, const char* password)
{
  // Disconnect from AP
  WiFiMode_t mode = WiFi.getMode();
  if (mode != WIFI_STA)
  {
    while(!WiFi.softAPdisconnect());  
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  bool tryToConnect = true;
  int tries = 0;
  while(tryToConnect)
  {
    int status = WiFi.status();
    if (status == WL_CONNECTED)
    {
      getConfiguration();
      tryToConnect = false;
    }
    tries++;
    delay(500);
    if (tries > 20) {
      tryToConnect = false;
      enableWifiAP();
    }
  }
}

bool tryWifiClient(const char* ssid, const char* password)
{
  // Si uso esta función es pq ya estoy en AP_STA
  // WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  
  int tries = 0;
  while(true)
  {
    int status = WiFi.status();
    if (status == WL_CONNECTED)
    {
      return true;
    }
    tries++;
    delay(500);
    if (tries > 20) {
      return false;
    }
  }
}
