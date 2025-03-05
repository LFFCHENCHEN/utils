#include "MqttClient.h"
#include <iostream>

int main() {
  MqttConfig config;
  config.id = "test_client";
  config.host = "localhost";
  config.port = 1883;

  MqttClient client(config);

  client.set_on_connect([](struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
      DBG(INFO) << "Connected successfully." << std::endl;
    } else {
      DBG(ERROR) << "Connect failed." << std::endl;
    }
  });

  client.set_on_disconnect([](struct mosquitto *mosq, void *obj, int rc) {
    DBG(INFO) << "Disconnected." << std::endl;
  });

  client.set_on_publish([](struct mosquitto *mosq, void *obj, int mid) {
    DBG(INFO) << "Message published (mid: " << mid << ")." << std::endl;
  });

  client.set_on_message([](struct mosquitto *mosq, void *obj,
                           const struct mosquitto_message *message) {
    DBG(INFO) << "Received message: " << (char *)message->payload
              << " on topic: " << message->topic << std::endl;
  });

  if (client.connect()) {
    DBG(INFO) << "Connecting to MQTT broker..." << std::endl;
  } else {
    DBG(ERROR) << "Failed to connect to MQTT broker." << std::endl;
    return 1;
  }

  // Publish a message
  std::string topic = "test/topic";
  std::string payload = "Hello, MQTT!";
  if (client.publish(topic, payload.c_str(), payload.size())) {
    DBG(INFO) << "Message published successfully." << std::endl;
  } else {
    DBG(ERROR) << "Failed to publish message." << std::endl;
  }

  // Subscribe to a topic
  if (client.subscribe(topic)) {
    DBG(INFO) << "Subscribed to topic: " << topic << std::endl;
  } else {
    DBG(ERROR) << "Failed to subscribe to topic." << std::endl;
  }

  // Start the loop to process callbacks
  while (true) {
    client.loop();
    // Perform other tasks here
  }

  return 0;
}
