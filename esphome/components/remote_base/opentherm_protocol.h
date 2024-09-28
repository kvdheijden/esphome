#pragma once

#include "esphome/core/component.h"
#include "remote_base.h"

namespace esphome {
namespace remote_base {

enum OpenthermMessageType : uint8_t {
  READ_DATA = 0x00,
  WRITE_DATA = 0x10,
  INVALID_DATA = 0x20,
  READ_ACK = 0x40,
  WRITE_ACK = 0x50,
  DATA_INVALID = 0x60,
  UNKNOWN_DATA_ID = 0x70
};

enum OpenthermMessageId : uint8_t {
  STATUS = 0,
  CH_SETPOINT = 1,
  CONTROLLER_CONFIG = 2,
  DEVICE_CONFIG = 3,
  COMMAND_CODE = 4,
  FAULT_FLAGS = 5,
  REMOTE = 6,
  COOLING_CONTROL = 7,
  CH2_SETPOINT = 8,
  CH_SETPOINT_OVERRIDE = 9,
  TSP_COUNT = 10,
  TSP_COMMAND = 11,
  FHB_SIZE = 12,
  FHB_COMMAND = 13,
  MAX_MODULATION_LEVEL = 14,
  MAX_BOILER_CAPACITY = 15,
  ROOM_SETPOINT = 16,
  MODULATION_LEVEL = 17,
  CH_WATER_PRESSURE = 18,
  DHW_FLOW_RATE = 19,
  DAY_TIME = 20,
  DATE = 21,
  YEAR = 22,
  ROOM_SETPOINT_CH2 = 23,
  ROOM_TEMP = 24,
  FEED_TEMP = 25,
  DHW_TEMP = 26,
  OUTSIDE_TEMP = 27,
  RETURN_WATER_TEMP = 28,
  SOLAR_STORE_TEMP = 29,
  SOLAR_COLLECT_TEMP = 30,
  FEED_TEMP_CH2 = 31,
  DHW2_TEMP = 32,
  EXHAUST_TEMP = 33,
  FAN_SPEED = 35,
  FLAME_CURRENT = 36,
  DHW_BOUNDS = 48,
  CH_BOUNDS = 49,
  OTC_CURVE_BOUNDS = 50,
  DHW_SETPOINT = 56,
  MAX_CH_SETPOINT = 57,
  OTC_CURVE_RATIO = 58,
  HVAC_STATUS = 70,
  REL_VENT_SETPOINT = 71,
  DEVICE_VENT = 74,
  REL_VENTILATION = 77,
  REL_HUMID_EXHAUST = 78,
  SUPPLY_INLET_TEMP = 80,
  SUPPLY_OUTLET_TEMP = 81,
  EXHAUST_INLET_TEMP = 82,
  EXHAUST_OUTLET_TEMP = 83,
  NOM_REL_VENTILATION = 87,
  OVERRIDE_FUNC = 100,
  OEM_DIAGNOSTIC = 115,
  BURNER_STARTS = 116,
  CH_PUMP_STARTS = 117,
  DHW_PUMP_STARTS = 118,
  DHW_BURNER_STARTS = 119,
  BURNER_HOURS = 120,
  CH_PUMP_HOURS = 121,
  DHW_PUMP_HOURS = 122,
  DHW_BURNER_HOURS = 123,
  OT_VERSION_CONTROLLER = 124,
  OT_VERSION_DEVICE = 125,
  VERSION_CONTROLLER = 126,
  VERSION_DEVICE = 127
};

struct OpenthermData {
  uint8_t type;
  uint8_t id;
  uint16_t data;

  /**
   * Construct an OpenthermData instance.
   *
   * @param type The message type (and possible parity bit).
   * @param id The message id.
   * @param data The message data.
   */
  OpenthermData(uint8_t type, uint8_t id, uint16_t data) : type(type), id(id), data(data) {}

  /**
   * Construct an empty OpenthermData instance.
   */
  OpenthermData() : OpenthermData(0, 0, 0) {}

  /**
   * Construct an OpenthermData instance from a raw value.
   *
   * @param raw The raw value which corresponds to an Opentherm message.
   */
  explicit OpenthermData(uint32_t raw)
      : OpenthermData(static_cast<uint8_t>(raw >> 24), static_cast<uint8_t>(raw >> 16), static_cast<uint16_t>(raw)) {}

  /**
   * Get a mask of bits of the message data in boolean format.
   *
   * @tparam length The length of the mask in bits.
   * @tparam position The position of the mask.
   *
   * @return The masked message data.
   */
  template<int length, int position> [[nodiscard]] uint16_t mask() const {
    return (data >> position) & ((1 << length) - 1);
  }

  /**
   * Set a mask of bits of the message data.
   *
   * @tparam length The length of the mask in bits.
   * @tparam position The position of the mask.
   * @param value The new value to be masked.
   */
  template<int length, int position> void mask(uint16_t value) {
    constexpr uint16_t m = (1 << length) - 1;
    data = (data & ~(m << position)) | ((value & m) << position);
  }

  /**
   * Get a bit of the message data in boolean format.
   *
   * @tparam position The position of the bit.
   *
   * @return The bit of the message data in boolean format.
   */
  template<int position> [[nodiscard]] bool flag() const { return static_cast<bool>(mask<1, position>()); }

  /**
   * Set a bit of the message data in boolean format.
   *
   * @tparam position The position of the bit.
   * @param value The new value in boolean format.
   */
  template<int position> void flag(bool value) { mask<1, position>(static_cast<uint16_t>(value)); }

  /**
   * Get a byte of the message data in unsigned (u8) format.
   *
   * @tparam position The position of the byte.
   *
   * @return The byte of the message data in u8 format.
   */
  template<int position> [[nodiscard]] uint8_t u8() const { return static_cast<uint8_t>(mask<8, position>()); }

  /**
   * Set a byte of the message data in unsigned (u8) format.
   *
   * @tparam position The position of the byte.
   * @param value The new value in u8 format.
   */
  template<int position> void u8(uint8_t value) { mask<8, position>(static_cast<uint16_t>(value)); }

  /**
   * Get a byte of the message data in signed (s8) format.
   *
   * @tparam position The position of the byte.
   *
   * @return The byte of the message data in s8 format.
   */
  template<int position> [[nodiscard]] int8_t s8() const {
    uint8_t tmp = u8<position>();
    return *reinterpret_cast<int8_t *>(&tmp);
  }

  /**
   * Set a byte of the message data in signed (s8) format.
   *
   * @tparam position The position of the byte.
   * @param value The new value in s8 format.
   */
  template<int position> void s8(int8_t value) { u8<position>(*reinterpret_cast<uint8_t *>(&value)); }

  /**
   * Get the message data in unsigned (u16) format.
   *
   * @return The message data in u16 format.
   */
  [[nodiscard]] uint16_t u16() const { return data; }

  /**
   * Set the message data in unsigned (u16) format.
   *
   * @param value The new value in u16 format.
   */
  void u16(uint16_t value) { data = value; }

  /**
   * Get the message data in signed (s16) format.
   *
   * @return The message data in s16 format.
   */
  [[nodiscard]] int16_t s16() const { return *reinterpret_cast<const int16_t *>(&data); }

  /**
   * Set the message data in signed (s16) format.
   *
   * @param value The new value in s16 format.
   */
  void s16(int16_t value) { data = *reinterpret_cast<uint16_t *>(&value); }

  /**
   * Get the message data in signed fixed point (q7.8) format.
   *
   * @return The message data in q7.8 format.
   */
  [[nodiscard]] float q7_8() const { return static_cast<float>(s16()) / 256.0f; }

  /**
   * Set the message data in signed fixed point (q7.8) format.
   *
   * @param value The new value in q7.8 format.
   */
  void q7_8(float value) { s16(static_cast<int16_t>(value * 256)); }

  /**
   * Add the parity bit to the OpenthermData
   */
  void calc_parity() {
    /* Check if parity is wrong (i.e. odd) */
    if (!check_parity()) {
      /* Parity is odd. Check if the parity bit should be added or removed. */
      constexpr uint8_t parity_mask = 0x80;

      if (type & parity_mask) {
        type &= ~parity_mask;
      } else {
        type |= parity_mask;
      }
    }
  }

  /**
   * Check if the parity is correct (i.e. even).
   *
   * @return True if the parity is correct. False otherwise.
   */
  [[nodiscard]] bool check_parity() const {
    int res = (type << 24) | (id << 16) | data;
    res ^= res >> 16;
    res ^= res >> 8;
    res ^= res >> 4;
    res ^= res >> 2;
    res ^= res >> 1;

    /* Return whether the current parity is correct (even) */
    return (~res) & 1;
  }

  bool operator==(const OpenthermData &rhs) const { return type == rhs.type && id == rhs.id && data == rhs.data; }
};

class OpenthermProtocol : public RemoteProtocol<OpenthermData> {
 public:
  void encode(RemoteTransmitData *dst, const OpenthermData &data) override;

  optional<OpenthermData> decode(RemoteReceiveData src) override;

  void dump(const OpenthermData &data) override;
};

DECLARE_REMOTE_PROTOCOL(Opentherm)

template<typename... Ts> class OpenthermAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(uint8_t, type)
  TEMPLATABLE_VALUE(uint8_t, id)
  TEMPLATABLE_VALUE(uint16_t, data)

  void encode(RemoteTransmitData *dst, Ts... x) override {
    OpenthermData data(this->type_.value(x...), this->id_.value(x...), this->data_.value(x...));

    /* Add parity bit */
    data.calc_parity();

    /* Encode */
    OpenthermProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
