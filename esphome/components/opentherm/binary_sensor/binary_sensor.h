#pragma once

#include "esphome/components/opentherm/opentherm_component.h"

#include "esphome/components/binary_sensor/binary_sensor.h"

#include "esphome/core/component.h"

namespace esphome {
namespace opentherm {

class OpenthermBinarySensor : public OpenthermBinaryComponent,
                              public binary_sensor::BinarySensor,
                              public PollingComponent {
 public:
  OpenthermBinarySensor();

  void setup() override;

  void build_request(remote_base::OpenthermData &data) override;

  void on_receive(const remote_base::OpenthermData &data) override;

  void on_error() override;

  void dump_config() override;

  void update() override;
};

}  // namespace opentherm
}  // namespace esphome
