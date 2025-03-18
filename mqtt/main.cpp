#include "MqttClient.hpp"
#include <iostream>

int main() {
  MqttConfig config;
  config.id = "test_client";
  config.host = "localhost";
  config.port = 1883;

  MqttClient client(config);

  client.set_on_connect([](struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
      APP_DBG("Connected successfully.");
    } else {
      APP_ERR("Connect failed.");
    }
  });

  client.set_on_disconnect([](struct mosquitto *mosq, void *obj, int rc) {
    APP_DBG("Disconnected.");
  });

  client.set_on_publish([](struct mosquitto *mosq, void *obj, int mid) {
    APP_DBG("Message published (mid: %d)", mid);
  });

  client.set_on_message([](struct mosquitto *mosq, void *obj,
                           const struct mosquitto_message *message) {
    APP_DBG("Received message: %s\n on topic: %s", (char *)message->payload,
            message->topic);
  });

  if (client.connect()) {
    APP_DBG("Connecting to MQTT broker...");
  } else {
    APP_ERR("Failed to connect to MQTT broker.");
    return -1;
  }

  // Publish a message
  std::string topic = "test/topic";
  std::string payload = "Hello, MQTT!";
  if (client.publish(topic, payload.c_str(), payload.size())) {
    APP_DBG("Message published successfully.");
  } else {
    APP_ERR("Failed to publish message.");
  }

  // Subscribe to a topic
  if (client.subscribe(topic)) {
    APP_DBG("Subscribed to topic: %s", topic.c_str());
  } else {
    APP_ERR("Failed to subscribe to topic.");
  }
  client.loop();
  // Start the loop to process callbacks
  while (true) {

    // Perform other tasks here
  }

  return 0;
}
