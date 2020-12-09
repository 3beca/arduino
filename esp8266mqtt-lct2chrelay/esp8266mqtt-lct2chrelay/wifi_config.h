#ifndef Wifi_Config_h
#define Wifi_Config_h
void initWifi();
void enableWifiAP();
void enableWifiClient(const char* ssid, const char* password);
bool tryWifiClient(const char* ssid, const char* password);
String getConfiguration();
String getConfigurationAP();
#endif
