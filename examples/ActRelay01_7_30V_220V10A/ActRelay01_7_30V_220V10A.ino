#include <Esp8266Mqtt.h>

// only runs once on boot
void setup() {

	//instatiates one object of they type Esp8266MQTT
	Esp8266Mqtt node(Esp8266Mqtt::ModuleType::MT_ActRelay01_7_30V_220V10A);

	//Assigns the topics where the module will send the data to
	node.setTopicActivation("home/secondFloor/bedroom/switch/temperature/heater");
	node.setTopicActivationStatus("home/secondFloor/bedroom/switch/temperature/heater/status");
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
