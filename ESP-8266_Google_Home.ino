#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h> 
#include <Adafruit_MQTT_Client.h>

#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#define WIFI_SSID "LoRa"//"Naam van je wifi"  
#define WIFI_PASS "vtitielt"//"Wachtwoord van je wifi"  

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "MaximLL"//"De naam van je account" 
#define MQTT_PASS "aio_yKba73v4Jhd8GNOMPKiG3VCSbPIE" 

ESP8266WebServer server;

int relay = D4; //LED_BUILTIN;
//int reset = D5;

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe Relay1 = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/Relay1"); //onoff");


void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  
  Serial.print("Nearby networks found  :");
  Serial.println(WiFi.scanNetworks());
    delay(500);
  Serial.println("List of surrounding Network SSIDs…:");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++)
  {
  Serial.println(WiFi.SSID(i));
  }

  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi");
   //WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println();
  Serial.print("Wifi Connected!");
    
    Serial.print("\n\n****************************");
    Serial.print("\nConnected to SSID          : ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address allotted to ESP : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address of ESP         : ");
    Serial.println(WiFi.macAddress());
    Serial.print("Signal strength is         : ");
    Serial.println(WiFi.RSSI());
    Serial.print("****************************");

  //Subscribe to the onoff topic
  mqtt.subscribe(&Relay1);

  pinMode(relay, OUTPUT);
}



void loop(){
  MQTT_connect();
  
  //Read from our subscription queue until we run out, or
  //wait up to 5 seconds for subscription to update
  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    //If we're in here, a subscription updated...
    if (subscription == &Relay1)
    {
      //Print the new value to the serial monitor
      Serial.print("onoff: ");
      Serial.println((char*) Relay1.lastread);
      
      //If the new value is  "ON", turn the light on.
      //Otherwise, turn it off.
      if (!strcmp((char*) Relay1.lastread, "ON"))
      {
        //Active low logic
        digitalWrite(relay, HIGH);
              }
      else
      {
        digitalWrite(relay, LOW);
        }

/*      if (!strcmp((char*) Relay1.lastread, "RESET"))
      {
        //Active low logic
        digitalWrite(reset, LOW);
      }
      else
      {
        digitalWrite(reset, HIGH);
      }*/
    }
  }

  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}


void MQTT_connect() 
{
  int8_t ret;
  // Stop if already connected
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("\n\nConnecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("\nRetrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) 
    ESP.restart();
    
  }
  Serial.println("\nMQTT Connected!");
}
