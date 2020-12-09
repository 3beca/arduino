#include "Arduino.h"
#include "lct2ch.h"
#include "lct_data.h"

LCT2CH::LCT2CH()
{
  statusRelayOne = OFF;
  statusRelayTwo = OFF;
}

void LCT2CH::connectDevice()
{
  Serial.begin(115200);
  
  // Engañamos al Relay diciendo que somos el ESP8266
  while(!connect2Relay());
  Serial.print("WIFI CONNECTED WIFI GOT IP AT+CIPMUX=1 AT+CIPSERVER=1,8080 AT+CIPSTO=360\r\n");

  // Esperamos a que el Relay se trague la mentira
  while ( !Serial.available () )
  {
    delay(50);
  }
  while ( Serial.available () )
  {
    String res = Serial.readStringUntil ('\n');
  }
}

bool LCT2CH::connect2Relay()
{
  // Esperamos a que nos pregunte el Relay
  while ( !Serial.available () )
  {
    delay(50);
  }
  String res = Serial.readStringUntil ('\n');
  if (res.indexOf("AT+RST") != -1) return true;
  return false;
}

Status LCT2CH::setRelayStatus(Relay relay, Status status)
{
  if (relay == ONE) {
    if (status == ON) {
      Serial.write (relON, sizeof(relON));
      statusRelayOne = status;
      return status;
    }
    if (status == OFF) {
      Serial.write (relOFF, sizeof(relON));
      statusRelayOne = status;
      return status;
    }
  }

  if (relay == TWO) {
    if (status == ON) {
      Serial.write (rel2ON, sizeof(relON));
      statusRelayTwo = status;
      return status;
    }
    if (status == Status::OFF) {
      Serial.write (rel2OFF, sizeof(relON));
      statusRelayTwo = status;
      return status;
    }
  }

  return NOTKNOWN;
}

Status LCT2CH::getRelayStatus(Relay relay)
{
   if (relay == ONE) {
     return statusRelayOne;
   }
   if (relay == TWO) {
     return statusRelayTwo;
   }
   return NOTKNOWN;
}
