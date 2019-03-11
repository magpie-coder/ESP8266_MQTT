//========================================================================================================================
//
// Library Esp8266Mqtt
//
// This library is essentially an interface that enables flash the ESP8266 modules with the smallest effort. The focus is 
// to enable more people to make MQTT integrations on their home automation hub, such as the well known Home Assistant.
//
// Main purposes for the development of this library:
//    - ease the implementation of MQTT communications on existent ESP8266 modules (temperature, humidity, relays, etc)
//    - reduce code duplication when programming multiple sensors for a project
//    - accelerate the implementation of the project by spending no time browsing for pins or configurations
//    
// This will help with integrating sensors and actuators on the Home Assistant project, no time needs to be spent
// browsing tring to find each pin and other properties of a module, instead just make an instance, add the topics,
// and flash.
//
// Example: flashing ten temperature sensor modules would otherwise mean to have 10 times the full length of code in
// separate files. This library includes all that repeated code, making it cleaner visually and possible for more people.
// See example files to visualise the use of this library.
//
// Ideally it will support more and more modules in the future, but for now starting with two: 
// - ESP8266 + DHT11 temperature sensor module. Example: http://bit.ly/mqtt_ha_2tW7 
// - ESP8266 + 7-30V powered 220V relay module. Example: http://bit.ly/mqtt_ha_2VOj
//
// Dependencies
// To use this library, the following libraries must be installed. They can be found on the libraries manager
// in the Arduino IDE
//    - Adafruit_Unified_Sensor
//    - DHT_sensor_library
//    - PubSubClient
//
// Questions, remarks or suggestions welcome on github or email: esp8266_mqtt@e.email
//
// Changelog:
//    20190310: initial release
//
//========================================================================================================================


#ifndef ESP8266MQTT_H
#define ESP8266MQTT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

extern int OUTPIN;
extern bool DEBUGMODE;
extern PubSubClient CLIENT;
extern char* TOPICACTIVATIONSTATUS;

class Esp8266Mqtt {

public:
   enum ModuleType {
      MT_SensTempDht11,             // ESP8266 + DHT11 temperature sensor module. Example: http://bit.ly/mqtt_ha_2tW7 
      MT_ActRelay01_7_30V_220V10A,  // ESP8266 + 7-30V powered 220V relay module. Example: http://bit.ly/mqtt_ha_2VOj
   };

   Esp8266Mqtt(ModuleType moduleType);
   void setTopicTemperature(char* newTopic);
   void setTopicHumidity(char* newTopic);
   void setTopicActivation(char* newTopic);
   void setTopicActivationStatus(char* newTopic);
   void setTimeMqttCheckInterval(uint miliseconds);
   void setDebugMode(bool debugMode);

   void loop();
   void reconnectMqtt();

private:
   ModuleType m_currentModule;
   char* m_topicTemperature;
   char* m_topicHumidity;
   char* m_topicActivation;
   bool m_definedTopicTemperature;
   bool m_definedTopicHumidity;
   bool m_definedTopicOutput;
   bool m_definedTopicActivation;
   bool m_definedTopicActivationStatus;
   uint m_timeMqttCheckInterval;
   char* m_nameMqttDeviceUnique;

   void setup_serial();
   void setup_wifi();
   void setup_device();
   void setup_mqtt();
   void loopSensTempDHT011();
   void loopSensActuator1();
   void setNameMqttDeviceUnique(char* nameUnique);
   bool checkBound(float newValue, float prevValue, float maxDiff);

   //Adjust here accordingly
   char* m_wifiSsid = "yourWifiSsid";
   char* m_wifiPassword= "yourWifiPass";
   char* m_mqttServer ="YourMQTTServerIP";
   char* m_mqttUser ="yourMqttUser";
   char* m_mqttPassword ="yourMqttPassword";

   WiFiServer m_wifiServer;
   WiFiClient m_espClient;
   DHT* m_dht;
};

#endif
