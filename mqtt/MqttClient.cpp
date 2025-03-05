#include "MqttClient.h"
#include <iostream>
#include <thread>
#include <chrono>

MqttClient::MqttClient(const MqttConfig& config)
    : id_(config.id), host_(config.host), port_(config.port), username_(config.username), password_(config.password),
      cafile_(config.cafile), certfile_(config.certfile), keyfile_(config.keyfile), mosq_(nullptr) {
    mosquitto_lib_init();
    mosq_ = mosquitto_new(id_.c_str(), true, this);
    if (mosq_) {
        mosquitto_connect_callback_set(mosq_, connect_callback);
        mosquitto_disconnect_callback_set(mosq_, disconnect_callback);
        mosquitto_publish_callback_set(mosq_, publish_callback);
        mosquitto_message_callback_set(mosq_, message_callback);

        if (!username_.empty() && !password_.empty()) {
            mosquitto_username_pw_set(mosq_, username_.c_str(), password_.c_str());
        }

        if (!cafile_.empty()) {
            mosquitto_tls_set(mosq_, cafile_.c_str(), nullptr, certfile_.c_str(), keyfile_.c_str(), nullptr);
        }
    }
}

MqttClient::~MqttClient() {
    disconnect();
    if (mosq_) {
        mosquitto_destroy(mosq_);
    }
    mosquitto_lib_cleanup();
}

bool MqttClient::connect() {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    int ret = mosquitto_connect(mosq_, host_.c_str(), port_, 60);
    if (ret == MOSQ_ERR_SUCCESS) {
        DBG(INFO) << "Connected to MQTT broker at " << host_ << ":" << port_ << std::endl;
    } else {
        DBG(ERROR) << "Failed to connect to MQTT broker at " << host_ << ":" << port_ << std::endl;
    }
    return ret == MOSQ_ERR_SUCCESS;
}

void MqttClient::disconnect() {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    mosquitto_disconnect(mosq_);
    DBG(INFO) << "Disconnected from MQTT broker" << std::endl;
}

bool MqttClient::publish(const std::string& topic, const void* payload, int payloadlen) {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    int ret = mosquitto_publish(mosq_, nullptr, topic.c_str(), payloadlen, payload, 1, false);
    if (ret == MOSQ_ERR_SUCCESS) {
        DBG(INFO) << "Published message to topic " << topic << std::endl;
    } else {
        DBG(ERROR) << "Failed to publish message to topic " << topic << std::endl;
        if (reconnect()) {
            ret = mosquitto_publish(mosq_, nullptr, topic.c_str(), payloadlen, payload, 1, false);
            if (ret == MOSQ_ERR_SUCCESS) {
                DBG(INFO) << "Published message to topic " << topic << " after reconnecting" << std::endl;
            } else {
                DBG(ERROR) << "Failed to publish message to topic " << topic << " after reconnecting" << std::endl;
            }
        }
    }
    return ret == MOSQ_ERR_SUCCESS;
}

bool MqttClient::subscribe(const std::string& topic) {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    int ret = mosquitto_subscribe(mosq_, nullptr, topic.c_str(), 1);
    if (ret == MOSQ_ERR_SUCCESS) {
        DBG(INFO) << "Subscribed to topic " << topic << std::endl;
    } else {
        DBG(ERROR) << "Failed to subscribe to topic " << topic << std::endl;
    }
    return ret == MOSQ_ERR_SUCCESS;
}

bool MqttClient::unsubscribe(const std::string& topic) {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    int ret = mosquitto_unsubscribe(mosq_, nullptr, topic.c_str());
    if (ret == MOSQ_ERR_SUCCESS) {
        DBG(INFO) << "Unsubscribed from topic " << topic << std::endl;
    } else {
        DBG(ERROR) << "Failed to unsubscribe from topic " << topic << std::endl;
    }
    return ret == MOSQ_ERR_SUCCESS;
}

bool MqttClient::reconnect() {
    std::lock_guard<std::mutex> lock(mosq_mutex_);
    mosquitto_reconnect(mosq_);
    int retries = 5;
    while (retries-- > 0) {
        int ret = mosquitto_reconnect(mosq_);
        if (ret == MOSQ_ERR_SUCCESS) {
            DBG(INFO) << "Reconnected successfully" << std::endl;
            return true;
        }
        DBG(WARNING) << "Reconnect attempt failed, retrying..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    DBG(ERROR) << "Reconnect failed after multiple attempts" << std::endl;
    return false;
}

void MqttClient::set_on_connect(std::function<void(struct mosquitto*, void*, int)> callback) {
    on_connect_ = callback;
}

void MqttClient::set_on_disconnect(std::function<void(struct mosquitto*, void*, int)> callback) {
    on_disconnect_ = callback;
}

void MqttClient::set_on_publish(std::function<void(struct mosquitto*, void*, int)> callback) {
    on_publish_ = callback;
}

void MqttClient::set_on_message(std::function<void(struct mosquitto*, void*, const struct mosquitto_message*)> callback) {
    on_message_ = callback;
}

void MqttClient::start() {
    // Removed infinite loop
    DBG(INFO) << "MQTT client started" << std::endl;
}

void MqttClient::loop(int timeout, int max_packets) {
    mosquitto_loop(mosq_, timeout, max_packets);
}

struct mosquitto* MqttClient::get_mosq() const {
    return mosq_;
}

void MqttClient::connect_callback(struct mosquitto *mosq, void *obj, int rc) {
    MqttClient* client = static_cast<MqttClient*>(obj);
    if (client->on_connect_) {
        client->on_connect_(mosq, obj, rc);
    }
}

void MqttClient::disconnect_callback(struct mosquitto *mosq, void *obj, int rc) {
    MqttClient* client = static_cast<MqttClient*>(obj);
    if (client->on_disconnect_) {
        client->on_disconnect_(mosq, obj, rc);
    }
}

void MqttClient::publish_callback(struct mosquitto *mosq, void *obj, int mid) {
    MqttClient* client = static_cast<MqttClient*>(obj);
    if (client->on_publish_) {
        client->on_publish_(mosq, obj, mid);
    }
}

void MqttClient::message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    MqttClient* client = static_cast<MqttClient*>(obj);
    if (client->on_message_) {
        client->on_message_(mosq, obj, message);
    }
}
