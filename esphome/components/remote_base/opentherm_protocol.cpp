#include "opentherm_protocol.h"

#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.opentherm";

static const uint32_t BIT_TIME_US = 500;
static const uint8_t NBITS = 32;

void OpenthermProtocol::encode(RemoteTransmitData *dst, const OpenthermData &data) {
  const uint32_t value = (data.type << 24) | (data.id << 16) | data.data;
  dst->set_carrier_frequency(2000);
  dst->reserve(2 * (NBITS + 2));

  /* Start bit */
  dst->item(BIT_TIME_US, BIT_TIME_US);

  for (uint32_t mask = 1UL << (NBITS - 1); mask != 0; mask >>= 1) {
    if (value & mask) {
      dst->mark(BIT_TIME_US);
      dst->space(BIT_TIME_US);
    } else {
      dst->space(BIT_TIME_US);
      dst->mark(BIT_TIME_US);
    }
  }

  /* Stop bit */
  dst->item(BIT_TIME_US, BIT_TIME_US);
}

optional<OpenthermData> OpenthermProtocol::decode(RemoteReceiveData src) {
  enum { TRANSITION, ZERO, ONE } state;
  uint32_t raw = 0;
  uint32_t mask = 1UL << (NBITS - 1);

  if (!src.expect_mark(BIT_TIME_US)) {
    return {};
  }

  if (src.expect_space(BIT_TIME_US)) {
    state = TRANSITION;
  } else if (src.expect_space(2 * BIT_TIME_US)) {
    state = ZERO;
  } else {
    return {};
  }

  while (mask != 0) {
    switch (state) {
      case TRANSITION:
        if (src.expect_space(BIT_TIME_US)) {
          state = ZERO;
        } else if (src.expect_mark(BIT_TIME_US)) {
          state = ONE;
        } else {
          return {};
        }
        break;
      case ZERO:
        raw &= ~mask;
        mask >>= 1;
        if (src.expect_mark(BIT_TIME_US)) {
          state = TRANSITION;
        } else if (src.expect_mark(2 * BIT_TIME_US)) {
          state = ONE;
        } else {
          return {};
        }
        break;
      case ONE:
        raw |= mask;
        mask >>= 1;
        if (src.expect_space(BIT_TIME_US)) {
          state = TRANSITION;
        } else if (src.expect_space(2 * BIT_TIME_US)) {
          state = ZERO;
        } else {
          return {};
        }
        break;
    }
  }

  if (state == TRANSITION)
    if (!src.expect_mark(BIT_TIME_US))
      return {};

  /* Ignore the last space */

  return OpenthermData{raw};
}

void OpenthermProtocol::dump(const OpenthermData &data) {
  ESP_LOGI(TAG, "Received Opentherm: type=0x%02X, id=0x%02X, data=0x%04X", data.type, data.id, data.data);
}

}  // namespace remote_base
}  // namespace esphome