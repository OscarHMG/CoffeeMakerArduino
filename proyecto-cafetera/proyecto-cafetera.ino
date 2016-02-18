// Maurice Ribble 
// 4-6-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code just lets you turn a digital out pin on and off.  That's
// all that is needed to verify a relay curcuit is working.
// Press the space bar to toggle the relay on and off.
#include <Bridge.h>
#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "Nexxt_0714E8"       //"CTI_DOMO"
#define PASSWORD    "S3c0D3P0ll0"  // "ct1esp0l15"
#define HOST_NAME   "api.thingspeak.com"
#define HOST_PORT   80

//El modulo ESP8266 usa SoftwareSerial...
SoftwareSerial mySerial(3,2); /* RX:D2, TX:D3 */
ESP8266 wifi(mySerial);

#define RELAY_PIN 4
String key = "JOXZJ0MWXKS66ENR"; //Key proporcionado por ThingSpeak
String head = "GET /update?api_key=";
String tail = " HTTP/1.0\r\n\r\n";
int estadoProximidad=0;

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  Serial.begin(9600); // open serial
    //Configura el módulo ESP8266
    Serial.print("setup begin\r\n");
    
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    while(!wifi.setOprToStation()) {
       Serial.print("to station + softap err\r\n");
    }
    Serial.print("to station + softap ok\r\n");
 
    while(!wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP failure\r\n");
    }
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    Serial.println( wifi.getLocalIP().c_str());    
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}

void loop()
{
  static int relayVal = 0;

  int sensorValue = analogRead(A5);
  // print out the value you read:
  Serial.println(sensorValue);
  delay(10);  

  if(sensorValue<=930){
    estadoProximidad=1;
    digitalWrite(RELAY_PIN, HIGH);
      sendProximidad();
    
    }else{
      estadoProximidad=0;
      digitalWrite(RELAY_PIN, LOW);
      sendProximidad();
      
    }
  /*int cmd = getOn_Off_State();

  
  Serial.print("ESTADO" +cmd);
    switch (cmd)
    {
    case 1:
      {
        digitalWrite(RELAY_PIN, HIGH);
      }
    case 0:
      {
        digitalWrite(RELAY_PIN, LOW);
      }
    }*/
}

void sendProximidad(void){
    uint8_t buffer[128] = {0};
    String field = "&field3=";
    String mensaje= head+key+field+ String(estadoProximidad) +tail;
    Serial.println(mensaje);
    
    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }

    const char *cmd= mensaje.c_str();
    wifi.send((const uint8_t*)cmd, strlen(cmd));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    delay(16000);
  }

int getOn_Off_State(){
  ("http://api.thingspeak.com/channels/81636/fields/1/last");
  uint8_t buffer[1024] = {0};

    if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }

    char *hello = "GET /channels/81636/fields/1/last";
    wifi.send((const uint8_t*)hello, strlen(hello));

    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    Serial.println("CODIGO:"+hello[0]);

    if (wifi.releaseTCP()) {
        Serial.print("release tcp ok\r\n");
    } else {
        Serial.print("release tcp err\r\n");
    }
    
  
  }
