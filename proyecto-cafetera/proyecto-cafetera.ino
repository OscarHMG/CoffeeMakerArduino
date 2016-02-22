// Maurice Ribble 
// 4-6-2008
// http://www.glacialwanderer.com/hobbyrobotics

// This code just lets you turn a digital out pin on and off.  That's
// all that is needed to verify a relay curcuit is working.
// Press the space bar to toggle the relay on and off.
#include <Bridge.h>
#include "ESP8266.h"
#include <SoftwareSerial.h>
#include "IRTemp.h"

static const byte PIN_DATA    = 4; // Choose any pins you like for these
static const byte PIN_CLOCK   = 5;
static const byte PIN_ACQUIRE = 6;

static const TempUnit SCALE=CELSIUS;  // Options are CELSIUS, FAHRENHEIT

IRTemp irTemp(PIN_ACQUIRE, PIN_CLOCK, PIN_DATA);

#define SSID     "Nexxt_0714E8" //"CTI_DOMO"  //   "moncayo CNT"    
#define PASSWORD   "S3c0D3P0ll0"//"ct1esp0l15"//   // "0912671898"
#define HOST_NAME   "api.thingspeak.com"
#define HOST_PORT   80

//El modulo ESP8266 usa SoftwareSerial...
SoftwareSerial mySerial(3,2); /* RX:D2, TX:D3 */
ESP8266 wifi(mySerial);

#define RELAY_PIN 8
 String estadOn_Off="";
int estadoProximidad=0;
float irTemperature=0;

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
       delay(1000);
    }
    Serial.print("to station + softap ok\r\n");
 
    while(!wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP failure\r\n");
    }
    Serial.print("Join AP success\r\n");
    Serial.print("IP:");
    Serial.println( wifi.getLocalIP().c_str());    
    
    while(!wifi.enableMUX()) {
        Serial.print("multiple error\r\n");
        delay(1000);
    } 
    Serial.print("multiple ok\r\n");
    
    Serial.print("setup end\r\n");
}

void loop()
{
  static int relayVal = 0;
  getOn_Off_State();
  //Proximity
  int sensorValue = analogRead(A5);
  // print out the value you read:
  Serial.print("Proximidad: ");
  Serial.println(sensorValue);
  delay(10);  
  //temperature:
  Serial.println("IRTemp example");
  Serial.println("~~~~~~~~~~~~~~");
   irTemperature = irTemp.getIRTemperature(SCALE);
  printTemperature("IR", irTemperature);

  float ambientTemperature = irTemp.getAmbientTemperature(SCALE);
  printTemperature("Ambient", ambientTemperature);

  delay(100);

    if(sensorValue<=930){
      estadoProximidad=1;
      
      }else{
        estadoProximidad=0;
      }
 
    //Serial.print("Estado: "+estadOn_Off);
    Serial.print("VEr estado:");
    Serial.println(estadOn_Off[0]);
    char estado=estadOn_Off[0];
      
    switch (estado)
    {
    case '1':
      {
        if(estadoProximidad==1 && irTemperature<90)
        digitalWrite(RELAY_PIN, HIGH);
        else
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("estado 1");
        break;    
      }
    case '0':
      {
        digitalWrite(RELAY_PIN, LOW);
        Serial.println("Estado 0");
        break;
      }
      default:
      {
        Serial.println(" MALO");
        break;
        }
    }
    estadOn_Off="";
    delay(500);
     sendDatos();
}

void sendDatos(){
    String field3 = "&field3=";
    String field2="&field2=";
    String mensaje= "GET http://api.thingspeak.com/update?api_key=JOXZJ0MWXKS66ENR"+field2+String(irTemperature,2)+field3+ String(estadoProximidad)+"HTTP/1.0\r\n\r\n\r\n";
    //Serial.println(mensaje);
    
   String cmd = "AT+CIPSTART=2,\"TCP\",\"";  //make this command: AT+CPISTART="TCP","146.227.57.195",80
    cmd += HOST_NAME;
    cmd += "\",80";
  
    mySerial.println(cmd);  //send command to device
    //mySerial.println("AT+ CIPMUX=1");
      delay(5000);  //wait a little while for 'Linked' response - this makes a difference
    if(mySerial.find("Linked"))  //message returned when connection established WEAK SPOT!! DOESN'T ALWAYS CONNECT
    {
      Serial.print("Connected to server for data at ");  //debug message
      Serial.println(HOST_NAME);
    }
    else
    {
      Serial.println("'Linked' response not received");  //weak spot! Need to recover elegantly
    }
    
       Serial.print(mensaje);
     
     //send 
     mySerial.print("AT+CIPSEND=2,");
     mySerial.println(mensaje.length());

  if(mySerial.find(">"))    //prompt offered by esp8266
  {
    //Serial.println("found > prompt - issuing GET request");  //a debug message 
    mySerial.println(mensaje);
    
  }
  else
  {
      //doesn't seem to work here?
    Serial.println("No '>' prompt received after AT+CPISEND");
  }

  if (mySerial.find("Status")) //get the date line from the http header (for example)
  {
    for (int i=0;i<30;i++)  //this should capture the 'Date: ' line from the header
    {
      if (mySerial.available())  //new cahracters received?
      {
        char c=mySerial.read();
         Serial.write(c);
      }else i--;  //if not, keep going round loop until we've got all the characters
    }
    
  }
  mySerial.println("AT+CIPCLOSE");
    delay(15000);
  }

void getOn_Off_State(void){
 String cmd = "AT+CIPSTART=1,\"TCP\",\"";  //make this command: AT+CPISTART="TCP","146.227.57.195",80
  cmd += HOST_NAME;
  cmd += "\",80";

  mySerial.println(cmd);  //send command to device
     // mySerial.println("AT+ CIPMUX=1");
    delay(5000);  //wait a little while for 'Linked' response - this makes a difference
  if(mySerial.find("Linked"))  //message returned when connection established WEAK SPOT!! DOESN'T ALWAYS CONNECT
  {
    Serial.print("Connected to server at ");  //debug message
    Serial.println(HOST_NAME);
  }
  else
  {
    Serial.println("'Linked' response not received");  //weak spot! Need to recover elegantly
  }
  
   String sendcommand =  "GET http://api.thingspeak.com/channels/81636/fields/1/last.json HTTP/1.0\r\n\r\n\r\n";  //construct http GET request
  //cmd += "Host: api.thingspeak.com\r\n\r\n";        //test file on my web
       Serial.print(sendcommand);
     
     //send 
     mySerial.print("AT+CIPSEND=1,");
     mySerial.println(sendcommand.length());

  if(mySerial.find(">"))    //prompt offered by esp8266
  {
    //Serial.println("found > prompt - issuing GET request");  //a debug message 
    mySerial.println(sendcommand);
    
  }
  else
  {
      //doesn't seem to work here?
    Serial.println("No '>' prompt received after AT+CPISEND");
  }

  if (mySerial.find("field1")) //get the date line from the http header (for example)
  {
    for (int i=0;i<7;i++)  //this should capture the 'Date: ' line from the header
    {
      if (mySerial.available())  //new cahracters received?
      {
        char c=mySerial.read();  //print to console
        if(c!='"'&& c!=':'&& c!='}'){
          estadOn_Off.concat(c);
          }
      }else i--;  //if not, keep going round loop until we've got all the characters
    }
    
  }
  mySerial.println("AT+CIPCLOSE");
  delay(5000);
  }

  
void printTemperature(
  char  *type,
  float  temperature) {

  Serial.print(type);
  Serial.print(" temperature: ");

  if (isnan(temperature)) {
    Serial.println("Failed");
  }
  else {
    Serial.print(temperature);
    Serial.println(SCALE == FAHRENHEIT  ?  " F"  :  " C");
  }
}
