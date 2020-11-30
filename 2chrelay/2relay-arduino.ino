/*
 *     Serial Monitor CR + LF (115200)
 *    /1/on Establece el rele 1 a on
 *    /1/off Establece el rele 1 a off
 *    /2/on Establece el rele 2 a on
 *    /2/off Establece el rele 2 a off
 */
#include <SoftwareSerial.h>

// Puerto serie por software
SoftwareSerial Relay(2, 3); // RX, TX

// Comandos a ejecutar por el relé
// Comando para cerrar Rele
byte relON[]  = {0xA0, 0x01, 0x01, 0xA2};
// Comando para abrir Rele
byte relOFF[] = {0xA0, 0x01, 0x00, 0xA1};
// Comando para cerrar Rele
byte rel2ON[]  = {0xA0, 0x02, 0x01, 0xA3};
// Comando para abrir Rele
byte rel2OFF[] = {0xA0, 0x02, 0x00, 0xA2};

void setup ()
{
  Serial.begin (115200);
  Relay.begin(115200);
  Serial.println("Starting communication...");

  // Esperamos a que nos pregunte el Relay
  while ( !Relay.available () )
  {
    delay(50);
  }
  String res = Relay.readStringUntil ('\n');
  Serial.print("Relay says: ");
  Serial.println(res);

  // Engañamos al Relay diciendo que somos el ESP8266
  Relay.print("WIFI CONNECTED WIFI GOT IP AT+CIPMUX=1 AT+CIPSERVER=1,8080 AT+CIPSTO=360\r\n");

  // Esperamos a que el Relay se trague la mentira
  while ( !Relay.available () )
  {
    delay(50);
  }
  while ( Relay.available () )
  {
    String res = Relay.readStringUntil ('\n');
    Serial.print("Relay says: ");
    Serial.println(res);
  }

  Serial.println("Communication actived");

}

void loop() {
  delay(500);

  // Capturamos informacion del rele
  while ( Relay.available () )
  {
    String res = Relay.readStringUntil ('\n');
    Serial.print("Relay says: ");
    Serial.println(res);
  }

  // Esperamos los comandos del monitor serie
  if ( Serial.available () )
  {
    //
    String req = Serial.readStringUntil ('\r');
    Relay.println(req);
    // Comprobamos los comandos del monitor serie
    Serial.print("Command received: ");
    Serial.println(req);
    if (req.indexOf ("/1/on") != -1)
    {
      Relay.write (relON, sizeof(relON));
      Serial.print("Reley data");
      Serial.write (relON, sizeof(relON));
    } else if (req.indexOf ("/1/off") != -1) {
        Relay.write (relOFF, sizeof(relOFF));
        Serial.print("Reley data");
        Serial.write (relOFF, sizeof(relOFF));
    } else if (req.indexOf ("/2/on") != -1) {
        Relay.write (rel2ON, sizeof(rel2ON));
        Serial.print("Reley data");
        Serial.write (rel2ON, sizeof(rel2ON));
    } else {
      if (req.indexOf ("/2/off") != -1)
        Relay.write (rel2OFF, sizeof(rel2OFF));
        Serial.print("Reley data");
        Serial.write (rel2OFF, sizeof(rel2OFF));
    }
    Serial.println();
    Serial.println("--- END ---");
    Relay.flush();
  }
}