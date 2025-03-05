#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <mosquitto.h>
#include <string>
#include <mutex>
#include <functional>
#include <iostream>

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

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Debug levels
#define INFO    1
#define WARNING 2
#define ERROR   3

// Debug macro
#define DBG(level) \
    if (level == INFO) std::cout << GREEN << "[INFO] " << RESET; \
    else if (level == WARNING) std::cout << YELLOW << "[WARNING] " << RESET; \
    else if (level == ERROR) std::cout << RED << "[ERROR] " << RESET; \
    std::cout

class MqttClient {
public:
    MqttClient(const MqttConfig& config);
    virtual ~MqttClient();

    bool connect();
    void disconnect();
    bool publish(const std::string& topic, const void* payload, int payloadlen);
    bool subscribe(const std::string& topic);
    bool unsubscribe(const std::string& topic);

    void set_on_connect(std::function<void(struct mosquitto*, void*, int)> callback);
    void set_on_disconnect(std::function<void(struct mosquitto*, void*, int)> callback);
    void set_on_publish(std::function<void(struct mosquitto*, void*, int)> callback);
    void set_on_message(std::function<void(struct mosquitto*, void*, const struct mosquitto_message*)> callback);

    void start();
    void loop(int timeout = -1, int max_packets = 1);

    struct mosquitto* get_mosq() const;

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

    std::function<void(struct mosquitto*, void*, int)> on_connect_;
    std::function<void(struct mosquitto*, void*, int)> on_disconnect_;
    std::function<void(struct mosquitto*, void*, int)> on_publish_;
    std::function<void(struct mosquitto*, void*, const struct mosquitto_message*)> on_message_;

    bool reconnect();

    static void connect_callback(struct mosquitto *mosq, void *obj, int rc);
    static void disconnect_callback(struct mosquitto *mosq, void *obj, int rc);
    static void publish_callback(struct mosquitto *mosq, void *obj, int mid);
    static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);
};

#endif // MQTTCLIENT_H
