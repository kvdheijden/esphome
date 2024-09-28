#pragma once

#include "esphome/components/opentherm/opentherm_component.h"

#include "esphome/components/switch/switch.h"

#include "esphome/core/component.h"

namespace esphome {
namespace opentherm {

class OpenthermSwitch : public OpenthermBinaryComponent, public switch_::Switch, public Component {
 public:
  OpenthermSwitch();

  void setup() override;

  void build_request(remote_base::OpenthermData &data) override;

  void on_receive(const remote_base::OpenthermData &data) override;

  bool assumed_state() override;

  void dump_config() override;

 protected:
  void write_state(bool state) override;
};

}  // namespace opentherm
}  // namespace esphome
