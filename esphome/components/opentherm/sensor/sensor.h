#pragma once

#include "esphome/components/opentherm/opentherm_component.h"

#include "esphome/components/sensor/sensor.h"

#include "esphome/core/component.h"

namespace esphome {
namespace opentherm {

class OpenthermSensor : public OpenthermFloatComponent, public sensor::Sensor, public PollingComponent {
 public:
  OpenthermSensor();

  void setup() override;

  void build_request(remote_base::OpenthermData &data) override;

  void on_receive(const remote_base::OpenthermData &data) override;

  void on_error() override;

  void dump_config() override;

  void update() override;
};

}  // namespace opentherm
}  // namespace esphome
