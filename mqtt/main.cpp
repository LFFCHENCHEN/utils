#include "MqttClient.hpp"
#include <iostream>
#include <unistd.h>
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
    APP_ERR("Disconnected.");
  });

  client.set_on_publish([](struct mosquitto *mosq, void *obj, int mid) {
    APP_DBG("Message published (mid: %d)", mid);
  });

  client.set_on_message([](struct mosquitto *mosq, void *obj,
                           const struct mosquitto_message *message) {
    APP_DBG("Received message:\n%s\non topic: %s", (char *)message->payload,
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

  // Subscribe to a topic
  if (client.subscribe(topic)) {
    APP_DBG("Subscribed to topic: %s", topic.c_str());
  } else {
    APP_ERR("Failed to subscribe to topic.");
  }
  client.loop();

  std::string payload = "Hello, MQTT!";
  if (client.publish(topic, payload.c_str(), payload.size())) {
    APP_DBG("Message published successfully.");
  } else {
    APP_ERR("Failed to publish message.");
  }

  // Start the loop to process callbacks
  while (true) {

    std::string payload = "Hello, MQTT!";
    if (client.publish(topic, payload.c_str(), payload.size())) {
      APP_DBG("Message published successfully.");
    } else {
      APP_ERR("Failed to publish message.");
    }
    sleep(1);
  }

  return 0;
}
