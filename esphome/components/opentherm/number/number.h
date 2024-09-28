#pragma once

#include "esphome/components/opentherm/opentherm_component.h"

#include "esphome/components/number/number.h"

#include "esphome/core/component.h"

namespace esphome {
namespace opentherm {

class OpenthermNumber : public OpenthermFloatComponent, public number::Number, public Component {
 public:
  OpenthermNumber();

  void setup() override;

  void build_request(remote_base::OpenthermData &data) override;

  void on_receive(const remote_base::OpenthermData &data) override;

  void on_error() override;

  void dump_config() override;

 protected:
  void control(float value) override;
};

}  // namespace opentherm
}  // namespace esphome
