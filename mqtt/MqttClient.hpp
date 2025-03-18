#ifndef MQTTCLIENT_HPP
#define MQTTCLIENT_HPP
#include "m_utils.hpp"
#include <functional>
#include <iostream>
#include <mosquitto.h>
#include <mutex>
#include <string>

struct MqttConfig {
  std::string id;
  std::string host;
  int port;
  std::string username;
  std::string password;
  std::string cafile;
  std::string certfile;
  std::string keyfile;
};

class MqttClient {
public:
  MqttClient(const MqttConfig &config);
  virtual ~MqttClient();

  bool connect();
  void disconnect();
  bool publish(const std::string &topic, const void *payload, int payloadlen);
  bool subscribe(const std::string &topic);
  bool unsubscribe(const std::string &topic);

  void
  set_on_connect(std::function<void(struct mosquitto *, void *, int)> callback);
  void set_on_disconnect(
      std::function<void(struct mosquitto *, void *, int)> callback);
  void
  set_on_publish(std::function<void(struct mosquitto *, void *, int)> callback);
  void set_on_message(std::function<void(struct mosquitto *, void *,
                                         const struct mosquitto_message *)>
                          callback);

  void start();
  void loop(int timeout = -1, int max_packets = 1);

  struct mosquitto *get_mosq() const;

private:
  std::string id_;
  std::string host_;
  int port_;
  std::string username_;
  std::string password_;
  std::string cafile_;
  std::string certfile_;
  std::string keyfile_;
  struct mosquitto *mosq_;
  std::mutex mosq_mutex_;

  std::function<void(struct mosquitto *, void *, int)> on_connect_;
  std::function<void(struct mosquitto *, void *, int)> on_disconnect_;
  std::function<void(struct mosquitto *, void *, int)> on_publish_;
  std::function<void(struct mosquitto *, void *,
                     const struct mosquitto_message *)>
      on_message_;

  bool reconnect();

  static void connect_callback(struct mosquitto *mosq, void *obj, int rc);
  static void disconnect_callback(struct mosquitto *mosq, void *obj, int rc);
  static void publish_callback(struct mosquitto *mosq, void *obj, int mid);
  static void message_callback(struct mosquitto *mosq, void *obj,
                               const struct mosquitto_message *message);
};

#endif // MQTTCLIENT_HPP
