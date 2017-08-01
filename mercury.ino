#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

// select which pin will trigger the configuration portal when set to LOW
// ESP-01 users please note: the only pins available (0 and 2), are shared 
// with the bootloader, so always set them HIGH at power-up
#define TRIGGER_PIN 12
#define LED_PIN 4
#define BUTTON_PIN 5
// MQTT
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/************************* MQTT Setup *********************************/
#define AIO_SERVER      "mercury.dwim.mx"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "dwim"
#define AIO_KEY         "gpscontrol1"


// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Publish button = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/button");
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
void MQTT_connect();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);
  pinMode(LED_PIN,OUTPUT);
  mqtt.subscribe(&onoffbutton);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}
uint32_t x=0;
void loop() {
  // is configuration portal requested?
  if ( digitalRead(TRIGGER_PIN) == HIGH ) {
    Serial.println("\n Pin Low");
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect("Mercury", "1234");
    

    //reset settings - for testing
    wifiManager.resetSettings();

    //sets timeout until configuration portal gets turned off
    //useful to make it all retry or go to sleep
    //in seconds
    wifiManager.setTimeout(120);

    //it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration

    //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
    WiFi.mode(WIFI_STA);
    
    if (!wifiManager.startConfigPortal("Mercury")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    
  }


  // put your main code here, to run repeatedly:
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
      if (strcmp((char *)onoffbutton.lastread,"on")==0)
      {
        //do stuff here because it was a match
        //digitalWrite(LED_PIN,HIGH);
        tone(LED_PIN,2500);
      }
      if (strcmp((char *)onoffbutton.lastread,"off")==0)
      {
        //do stuff here because it was a match
        //digitalWrite(LED_PIN,LOW);
        noTone(LED_PIN);
      }

    }
  }
  // Now we can publish stuff!
  Serial.print(F("\nSending button val "));
  Serial.print(digitalRead(BUTTON_PIN));
  Serial.print("...");
  if (! button.publish(digitalRead(BUTTON_PIN))) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
  
  
}

