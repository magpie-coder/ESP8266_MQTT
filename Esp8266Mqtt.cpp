
#include "Esp8266Mqtt.h"

PubSubClient CLIENT;
int OUTPIN = 0;
bool DEBUGMODE = false;
char* TOPICACTIVATIONSTATUS;

// function called when a new mqtt message has harrived on a subscribed topic
void callbackAct1(char* topic, byte* payload, unsigned int length)
{
   if(DEBUGMODE)
   {
      Serial.print("Message arrived [");
      Serial.print(topic);
      Serial.println("] ");
   }

   for (int i = 0; i < length; i++)
   {
      char receivedChar = (char)payload[i];
      Serial.print(receivedChar);
      if (receivedChar == '1') {
         digitalWrite(OUTPIN, HIGH);
         if(DEBUGMODE){Serial.println(" Setting output to high");}
         CLIENT.publish(TOPICACTIVATIONSTATUS, String(1).c_str(), true);
      } else if (receivedChar == '0') {
         digitalWrite(OUTPIN, LOW);
         if(DEBUGMODE){Serial.println(" Setting output to low");}
         CLIENT.publish(TOPICACTIVATIONSTATUS, String(0).c_str(), true);
      }
      break;
   }
   Serial.println();
}

Esp8266Mqtt::Esp8266Mqtt(ModuleType moduleType):
m_currentModule(moduleType),
m_topicTemperature("sensor/temperature"),
m_topicHumidity("sensor/humidity"),
m_topicActivation("actuator/output/request"),
m_definedTopicTemperature(false),
m_definedTopicHumidity(false),
m_definedTopicActivation(false),
m_definedTopicActivationStatus(false),
m_timeMqttCheckInterval(1000),
m_nameMqttDeviceUnique("ChangeMe"),
m_wifiServer(80)
{
   setup_serial();
   setup_wifi();
   setup_mqtt();
   setup_device();
};

void Esp8266Mqtt::setup_mqtt()
{
   // finds a unique name for the ESP chip in
   String id = (String)system_get_chip_id();
   char idc[32];
   id.toCharArray(idc,32);
   setNameMqttDeviceUnique(idc);
   CLIENT.setClient(m_espClient);
   CLIENT.setServer(m_mqttServer, 1883);
}

void Esp8266Mqtt::setup_device()
{
   if(m_currentModule == MT_SensTempDht11)
   {
      #define DHTTYPE DHT11  // DHT 11

      const int DHTPin = 2;
      // Initialize DHT sensor.
      DHT dht(DHTPin, DHTTYPE);
      m_dht = &dht;
      m_dht->begin();

      Serial.println("Connected to temp/humidity sensor DHT11.");
      delay(10);
   }
   else if(m_currentModule == MT_ActRelay01_7_30V_220V10A)
   {
      OUTPIN = 4;
      pinMode(OUTPIN, OUTPUT);
      CLIENT.setCallback(&callbackAct1);
   }
}

void Esp8266Mqtt::setDebugMode(bool debugMode)
{
   DEBUGMODE = debugMode;
}
void Esp8266Mqtt::setTopicTemperature(char* newTopic)
{
   m_definedTopicTemperature = true;
   m_topicTemperature = newTopic;
}
void Esp8266Mqtt::setTopicHumidity(char* newTopic)
{
   m_definedTopicHumidity = true;
   m_topicHumidity = newTopic;
}
void Esp8266Mqtt::setTopicActivation(char* newTopic)
{
   m_definedTopicActivation = true;
   m_topicActivation = newTopic;
}
void Esp8266Mqtt::setTopicActivationStatus(char* newTopic)
{
   m_definedTopicActivationStatus = true;
   TOPICACTIVATIONSTATUS = newTopic;
}

void Esp8266Mqtt::setTimeMqttCheckInterval(uint miliseconds)
{
   m_timeMqttCheckInterval = miliseconds;
}
void Esp8266Mqtt::setNameMqttDeviceUnique(char* nameUnique)
{
   if(DEBUGMODE)
   {
      Serial.print("nameUnique is: ");
      Serial.println(nameUnique);
   }
   m_nameMqttDeviceUnique = nameUnique;
}

void Esp8266Mqtt::setup_serial()
{
   Serial.begin(115200);//baudRate
   delay(10);
   Serial.println("Connected to Serial!");
   Serial.println("Baudrate: 115200");
}

void Esp8266Mqtt::setup_wifi()
{
   delay(10);
   // We start by connecting to a WiFi network
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(m_wifiSsid);

   WiFi.begin(m_wifiSsid, m_wifiPassword);

   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }

   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
}

void Esp8266Mqtt::reconnectMqtt() {
   // Loop until we're reconnected
   if(DEBUGMODE){Serial.println("Checking for MQTT connection");}
   while (!CLIENT.connected()) {
      Serial.print("Attempting MQTT connection...");
      // Attempt to connect
      if (CLIENT.connect(m_nameMqttDeviceUnique, m_mqttUser, m_mqttPassword))
      {
         Serial.println("device " + (String)m_nameMqttDeviceUnique + "connected");
         CLIENT.subscribe(m_topicActivation);
         int state = digitalRead(OUTPIN);
         CLIENT.publish(TOPICACTIVATIONSTATUS, String(state).c_str(), true);
      }
      else
      {
         Serial.print("failed, rc=");
         Serial.print(CLIENT.state());
         Serial.println(" try again in 5 seconds");
         // Wait 5 seconds before retrying
         delay(5000);
      }
   }
}

bool Esp8266Mqtt::checkBound(float newValue, float prevValue, float maxDiff)
{
   return !isnan(newValue) &&
   (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

void Esp8266Mqtt::loop()
{
   switch(m_currentModule)
   {
      case MT_SensTempDht11: loopSensTempDHT011();
      break;
      case MT_ActRelay01_7_30V_220V10A : loopSensActuator1();
      break;
   }
   delay(1000);
}

void Esp8266Mqtt::loopSensActuator1()
{
   reconnectMqtt();
   CLIENT.loop();
}

void Esp8266Mqtt::loopSensTempDHT011() 
{
   reconnectMqtt();
   CLIENT.loop();

   float temp = 0.0;
   float hum = 0.0;
   float diff = 0.1;

   float newTemp = m_dht->readTemperature();
   float newHum = m_dht->readHumidity();

   if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;

      if(DEBUGMODE){Serial.print("New temperature:");}
      if(DEBUGMODE){Serial.println(String(temp).c_str());}
      CLIENT.publish(m_topicTemperature, String(temp).c_str(), true);
   }

   if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      if(DEBUGMODE){Serial.print("New humidity:");}
      if(DEBUGMODE){Serial.println(String(hum).c_str());}
      CLIENT.publish(m_topicHumidity, String(hum).c_str(), true);
   }

   delay(m_timeMqttCheckInterval);
}
