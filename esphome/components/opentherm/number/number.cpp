#include "number.h"

#include "esphome/components/opentherm/opentherm_hub.h"

#include "esphome/core/log.h"

namespace esphome {
namespace opentherm {

constexpr const char *TAG = "number.opentherm";

OpenthermNumber::OpenthermNumber() : OpenthermFloatComponent(), number::Number(), Component() {}

void OpenthermNumber::setup() {
  if (!parent_) {
    mark_failed();
  } else {
    parent_->register_component(this);
    parent_->enqueue(remote_base::READ_DATA, message_id);
    ESP_LOGD(TAG, "%s initialized", get_object_id().c_str());
  }
}

void OpenthermNumber::build_request(remote_base::OpenthermData &data) {
  if (write_callback)
    write_callback(data, state);
}

void OpenthermNumber::on_receive(const remote_base::OpenthermData &data) {
  if (data.id != message_id) {
    ESP_LOGI(TAG, "Invalid message id: 0x%02X (expected 0x%02X)", data.id, message_id);
    has_state_ = false;
  } else if (read_callback) {
    publish_state(read_callback(data));
  } else {
    has_state_ = false;
  }
}

void OpenthermNumber::on_error() { has_state_ = false; }

void OpenthermNumber::dump_config() {
  LOG_NUMBER("", "OpenthermNumber", this);
  ESP_LOGCONFIG(TAG, "  Message Type: 0x%02X", message_type);
  ESP_LOGCONFIG(TAG, "  Message ID: 0x%02X", message_id);
  if (read_callback)
    ESP_LOGCONFIG(TAG, "  Read callback: %s", read_callback_name.c_str());
  if (write_callback)
    ESP_LOGCONFIG(TAG, "  Write callback: %s", write_callback_name.c_str());
}

void OpenthermNumber::control(float value) {
  publish_state(value);
  parent_->enqueue(message_type, message_id);
}

}  // namespace opentherm
}  // namespace esphome