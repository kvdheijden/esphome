#include "sensor.h"

#include "esphome/components/opentherm/opentherm_hub.h"

#include "esphome/core/log.h"

namespace esphome {
namespace opentherm {

constexpr const char *TAG = "sensor.opentherm";

OpenthermSensor::OpenthermSensor() : OpenthermFloatComponent(), sensor::Sensor(), PollingComponent() {}

void OpenthermSensor::setup() {
  if (!parent_) {
    mark_failed();
  } else {
    parent_->register_component(this);
    ESP_LOGD(TAG, "%s initialized", get_object_id().c_str());
  }
}

void OpenthermSensor::build_request(remote_base::OpenthermData &data) {
  if (write_callback)
    write_callback(data, state);
}

void OpenthermSensor::on_receive(const remote_base::OpenthermData &data) {
  if (data.id != message_id) {
    ESP_LOGI(TAG, "Invalid message id: 0x%02X (expected 0x%02X)", data.id, message_id);
    has_state_ = false;
  } else if (read_callback) {
    publish_state(read_callback(data));
  } else {
    has_state_ = false;
  }
}

void OpenthermSensor::on_error() { has_state_ = false; }

void OpenthermSensor::dump_config() {
  LOG_SENSOR("", "OpenthermSensor", this);
  ESP_LOGCONFIG(TAG, "  Message Type: 0x%02X", message_type);
  ESP_LOGCONFIG(TAG, "  Message ID: 0x%02X", message_id);
  if (read_callback)
    ESP_LOGCONFIG(TAG, "  Read callback: %s", read_callback_name.c_str());
  if (write_callback)
    ESP_LOGCONFIG(TAG, "  Write callback: %s", write_callback_name.c_str());
  ESP_LOGCONFIG(TAG, "  Update interval: %ums", update_interval_);
}

void OpenthermSensor::update() { parent_->enqueue(message_type, message_id); }

}  // namespace opentherm
}  // namespace esphome