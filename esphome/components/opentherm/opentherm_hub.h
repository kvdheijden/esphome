#pragma once

#include "esphome/core/hal.h"

#include "esphome/components/remote_transmitter/remote_transmitter.h"
#include "esphome/components/remote_receiver/remote_receiver.h"
#include "esphome/components/remote_base/opentherm_protocol.h"

#include "opentherm_component.h"
#include "unique_queue.h"

#include <unordered_set>

namespace esphome {
namespace opentherm {

struct OpenthermDataHash {
  std::size_t operator()(const remote_base::OpenthermData &data) const {
    std::size_t seed = 0;
    hash(seed, data.type);
    hash(seed, data.id);
    hash(seed, data.data);
    return seed;
  }

 private:
  template<typename T> inline void hash(std::size_t &seed, const T &t) const {
    std::hash<T> hasher;
    seed ^= hasher(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};

struct OpenthermDataComp {
  constexpr static uint8_t TYPE_MASK = 0x70;
  bool operator()(const remote_base::OpenthermData &a, const remote_base::OpenthermData &b) const {
    uint8_t a_type = a.type & TYPE_MASK;
    uint8_t b_type = b.type & TYPE_MASK;
    return a_type == b_type && a.id == b.id;
  }
};

class OpenthermHub : public remote_base::RemoteReceiverListener, public Component {
 private:
  /** TODO */
  remote_transmitter::RemoteTransmitterComponent *transmitter{nullptr};

  /** TODO */
  remote_receiver::RemoteReceiverComponent *receiver{nullptr};

  /** TODO */
  enum { IDLE, PRE_RECEIVE, RECEIVING, POST_RECEIVE } state{IDLE};

  /** TODO */
  std::unordered_set<OpenthermBaseComponent *> components{};

  /** TODO */
  remote_base::OpenthermProtocol protocol{};

  /** TODO */
  unique_queue<remote_base::OpenthermData, OpenthermDataHash, OpenthermDataComp> queue{};

  bool recv();

 public:
  OpenthermHub();

  void setup() override;

  void set_transmitter(remote_transmitter::RemoteTransmitterComponent *transmitter_) {
    this->transmitter = transmitter_;
  }

  void set_receiver(remote_receiver::RemoteReceiverComponent *receiver_) { this->receiver = receiver_; }

  void loop() override;

  bool on_receive(remote_base::RemoteReceiveData data) override;

  void register_component(OpenthermBaseComponent *component);

  void enqueue(uint8_t message_type, uint8_t message_id);

  void dump_config() override;

  [[nodiscard]] float get_setup_priority() const override { return setup_priority::HARDWARE; }
};

}  // namespace opentherm
}  // namespace esphome
