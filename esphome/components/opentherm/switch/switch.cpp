#include "switch.h"

#include "esphome/components/opentherm/opentherm_hub.h"

#include "esphome/core/log.h"

namespace esphome {
namespace opentherm {

constexpr const char *TAG = "switch.opentherm";

OpenthermSwitch::OpenthermSwitch() : OpenthermBinaryComponent(), switch_::Switch(), Component() {}

void OpenthermSwitch::setup() {
  if (!parent_) {
    mark_failed();
  } else {
    parent_->register_component(this);
    ESP_LOGD(TAG, "%s initialized", get_object_id().c_str());
  }
}

void OpenthermSwitch::build_request(remote_base::OpenthermData &data) {
  if (write_callback)
    write_callback(data, state);
}

void OpenthermSwitch::on_receive(const remote_base::OpenthermData &data) {
  if (data.id != message_id) {
    ESP_LOGI(TAG, "Invalid message id: 0x%02X (expected 0x%02X)", data.id, message_id);
  } else if (read_callback) {
    publish_state(read_callback(data));
  }
}

bool OpenthermSwitch::assumed_state() { return !read_callback; }

void OpenthermSwitch::dump_config() {
  LOG_SWITCH("", "OpenthermSwitch", this);
  ESP_LOGCONFIG(TAG, "  Message Type: 0x%02X", message_type);
  ESP_LOGCONFIG(TAG, "  Message ID: 0x%02X", message_id);
  if (read_callback)
    ESP_LOGCONFIG(TAG, "  Read callback: %s", read_callback_name.c_str());
  if (write_callback)
    ESP_LOGCONFIG(TAG, "  Write callback: %s", write_callback_name.c_str());
}

void OpenthermSwitch::write_state(bool state) {
  publish_state(state);
  parent_->enqueue(message_type, message_id);
}

}  // namespace opentherm
}  // namespace esphome