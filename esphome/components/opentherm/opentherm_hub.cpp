#include "opentherm_hub.h"

#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace opentherm {

constexpr static const char *TAG = "opentherm";
constexpr static const char *RECEIVING_NAME = "receiving";
constexpr static const char *MAX_TIMEOUT_NAME = "max_timeout";
constexpr static uint32_t RECEIVING_TIMEOUT = 834;
constexpr static uint32_t PRE_RECEIVE_TIMEOUT = 20;
constexpr static uint32_t POST_RECEIVE_TIMEOUT = 100;
constexpr static uint32_t MAX_TIMEOUT = 1150;

OpenthermHub::OpenthermHub() : remote_base::RemoteReceiverListener(), Component() {}

void OpenthermHub::setup() {
  if (transmitter == nullptr || receiver == nullptr) {
    mark_failed();
  } else {
    receiver->register_listener(this);
    ESP_LOGI(TAG, "Opentherm hub initialized");
  }
}

void OpenthermHub::loop() {
  if (state == IDLE) {
    cancel_timeout(MAX_TIMEOUT_NAME);

    if (!queue.empty()) {
      remote_base::OpenthermData data = queue.pop_front();

      state = PRE_RECEIVE;

      set_timeout(MAX_TIMEOUT_NAME, MAX_TIMEOUT, [this] {
        ESP_LOGW(TAG, "Message timeout");
        state = IDLE;
      });

      auto call = transmitter->transmit();
      for (auto component : components) {
        if ((component->message_id == data.id) &&
            (static_cast<uint8_t>(component->message_type) == (data.type & OpenthermDataComp::TYPE_MASK))) {
          component->build_request(data);
        }
        App.feed_wdt();
      }

      data.calc_parity();

      ESP_LOGD(TAG, "Send Opentherm: type=0x%02X, id=0x%02X, data=0x%04X", data.type, data.id, data.data);
      protocol.encode(call.get_data(), data);
      call.perform();

      set_timeout(PRE_RECEIVE_TIMEOUT, [this] { state = RECEIVING; });

      set_timeout(RECEIVING_NAME, RECEIVING_TIMEOUT, [this, data] {
        for (auto component : components) {
          if (component->message_id == data.id) {
            component->on_error();
          }
          App.feed_wdt();
        }

        ESP_LOGW(TAG, "Receive timeout");
        state = IDLE;
      });
    }
  }
}

bool OpenthermHub::recv() {
  state = POST_RECEIVE;
  set_timeout(POST_RECEIVE_TIMEOUT, [this] {
    ESP_LOGD(TAG, "Message done, remaining message: %u", queue.size());
    state = IDLE;
  });

  return true;
}

bool OpenthermHub::on_receive(remote_base::RemoteReceiveData data) {
  optional<remote_base::OpenthermData> ot;

  if (state != RECEIVING)
    return false;

  cancel_timeout(RECEIVING_NAME);

  ot = protocol.decode(data);

  /* Check if the data was valid */
  if (!ot.has_value())
    return false;

  /* Check if the parity was valid */
  if (!ot->check_parity()) {
    ESP_LOGW(TAG, "Invalid parity");

    for (auto component : components) {
      if (component->message_id == ot->id) {
        component->on_error();
      }

      App.feed_wdt();
    }
  } else if ((ot->type & 0x70) == remote_base::DATA_INVALID) {
    ESP_LOGW(TAG, "Data invalid: id=0x%02X, data=0x%04X", ot->id, ot->data);
  } else if ((ot->type & 0x70) == remote_base::UNKNOWN_DATA_ID) {
    ESP_LOGW(TAG, "Unknown data id: 0x%02X", ot->id);
  } else {
    ESP_LOGD(TAG, "Received Opentherm: type=0x%02X, id=0x%02X, data=0x%04X", ot->type, ot->id, ot->data);

    for (auto component : components) {
      if (component->message_id == ot->id) {
        component->on_receive(ot.value());
      }

      App.feed_wdt();
    }
  }

  return recv();
}

void OpenthermHub::register_component(OpenthermBaseComponent *component) { components.insert(component); }

void OpenthermHub::enqueue(uint8_t message_type, uint8_t message_id) {
  if (queue.emplace(message_type, message_id, 0)) {
    ESP_LOGI(TAG, "Enqueued message with type 0x%02X, id 0x%02X", message_type, message_id);
  } else {
    ESP_LOGI(TAG, "Message with type 0x%02X, id 0x%02X not unique", message_type, message_id);
  }
}

void OpenthermHub::dump_config() {
  ESP_LOGCONFIG(TAG, "Opentherm:");
  ESP_LOGCONFIG(TAG, "  Total number of components: %u", components.size());
}

}  // namespace opentherm
}  // namespace esphome