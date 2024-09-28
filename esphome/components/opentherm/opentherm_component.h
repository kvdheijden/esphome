#pragma once

#include "esphome/components/remote_base/opentherm_protocol.h"

#include <type_traits>

namespace esphome {
namespace opentherm {

class OpenthermHub;

class OpenthermBaseComponent : public Parented<OpenthermHub> {
 public:
  uint8_t message_id{0};

  remote_base::OpenthermMessageType message_type{remote_base::OpenthermMessageType::INVALID_DATA};

  std::string read_callback_name{};

  std::string write_callback_name{};

  virtual void build_request(remote_base::OpenthermData &data) = 0;

  virtual void on_receive(const remote_base::OpenthermData &data) = 0;

  virtual void on_error() {};

  void set_message_id(uint8_t message_id_) { message_id = message_id_; }

  void set_message_type(remote_base::OpenthermMessageType message_type_) { message_type = message_type_; }
};

class OpenthermBinaryComponent : public OpenthermBaseComponent {
 public:
  using read_callback_t = std::function<bool(const remote_base::OpenthermData &)>;
  using write_callback_t = std::function<void(remote_base::OpenthermData &, bool)>;

  read_callback_t read_callback{nullptr};

  write_callback_t write_callback{nullptr};

  template<typename T> void set_read_callback(T (remote_base::OpenthermData:: *fn)() const, std::string name) {
    if (nullptr == fn) {
      read_callback = {};
      read_callback_name = {};
    } else {
      read_callback = [fn](const remote_base::OpenthermData &data) { return static_cast<bool>((data.*fn)()); };
      read_callback_name = std::move(name);
    }
  }

  template<typename T> void set_write_callback(void (remote_base::OpenthermData:: *fn)(T), std::string name) {
    if (nullptr == fn) {
      write_callback = {};
      write_callback_name = {};
    } else {
      write_callback = [fn](remote_base::OpenthermData &data, bool state) { (data.*fn)(static_cast<T>(state)); };
      write_callback_name = std::move(name);
    }
  }
};

class OpenthermFloatComponent : public OpenthermBaseComponent {
 public:
  using read_callback_t = std::function<float(const remote_base::OpenthermData &)>;
  using write_callback_t = std::function<void(remote_base::OpenthermData &, float)>;

  read_callback_t read_callback{};

  write_callback_t write_callback{};

  template<typename T> void set_read_callback(T (remote_base::OpenthermData:: *fn)() const, std::string name) {
    if (nullptr == fn) {
      read_callback = {};
      read_callback_name = {};
    } else {
      read_callback = [fn](const remote_base::OpenthermData &data) { return static_cast<float>((data.*fn)()); };
      read_callback_name = std::move(name);
    }
  }

  template<typename T> void set_write_callback(void (remote_base::OpenthermData:: *fn)(T), std::string name) {
    if (nullptr == fn) {
      write_callback = {};
      write_callback_name = {};
    } else {
      write_callback = [fn](remote_base::OpenthermData &data, float state) { (data.*fn)(static_cast<T>(state)); };
      write_callback_name = std::move(name);
    }
  }
};

}  // namespace opentherm
}  // namespace esphome