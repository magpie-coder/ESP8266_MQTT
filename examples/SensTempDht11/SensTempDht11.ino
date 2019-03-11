#include <Esp8266Mqtt.h>

// only runs once on boot
void setup() 
{
	//instatiates one object of the type Esp8266MQTT
	Esp8266Mqtt node(Esp8266Mqtt::ModuleType::MT_SensTempDht11);

	//Assigns the topics where the module will send the data to
	node.setTopicTemperature("home/groundFloor/livingRoom/sensor/temperature");
	node.setTopicHumidity("home/groundFloor/livingRoom/sensor/humidity");
	node.setTimeMqttCheckInterval(1000);


	while (true)
	{
	node.loop();
	}

	// Dont forget to adjust the .h file with your wifi and mqtt server data!
}

void loop()
{
}