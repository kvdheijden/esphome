import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import remote_base, remote_transmitter, remote_receiver
from esphome.components.opentherm.const import (
    CONF_OPENTHERM_ID,
    CONF_MESSAGE_ID,
    CONF_MESSAGE_TYPE,
    CONF_READ_CALLBACK,
    CONF_WRITE_CALLBACK,
    CONF_CALLBACK
)
from esphome.components.remote_base import (
    CONF_TRANSMITTER_ID,
    CONF_RECEIVER_ID,
    OpenthermData, OPENTHERM_MESSAGE_TYPE, OPENTHERM_MESSAGE_ID, )
from esphome.const import (
    CONF_ID,
    CONF_POSITION,
    CONF_LENGTH
)
from esphome.cpp_generator import MockObj

CODEOWNERS = ["@kvdheijden"]
MULTI_CONF = True

opentherm_ns = cg.esphome_ns.namespace("opentherm")
OpenthermHub = opentherm_ns.class_("OpenthermHub", remote_base.RemoteReceiverListener, cg.Component)
OpenthermComponent = opentherm_ns.class_("OpenthermComponent", cg.Parented.template(OpenthermHub))
OpenthermBinaryComponent = opentherm_ns.class_("OpenthermBinaryComponent", OpenthermComponent)
OpenthermFloatComponent = opentherm_ns.class_("OpenthermFloatComponent", OpenthermComponent)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(OpenthermHub),
        cv.GenerateID(CONF_TRANSMITTER_ID): cv.use_id(remote_transmitter.RemoteTransmitterComponent),
        cv.GenerateID(CONF_RECEIVER_ID): cv.use_id(remote_receiver.RemoteReceiverComponent),
    })
    .extend(cv.COMPONENT_SCHEMA)
)

_static_OpenthermData = MockObj(OpenthermData, '::')

CALLBACK_ALIASES = {
    'flag0': ('flag', 0),
    'flag1': ('flag', 1),
    'flag2': ('flag', 2),
    'flag3': ('flag', 3),
    'flag4': ('flag', 4),
    'flag5': ('flag', 5),
    'flag6': ('flag', 6),
    'flag7': ('flag', 7),
    'flag8': ('flag', 8),
    'flag9': ('flag', 9),
    'flag10': ('flag', 10),
    'flag11': ('flag', 11),
    'flag12': ('flag', 12),
    'flag13': ('flag', 13),
    'flag14': ('flag', 14),
    'flag15': ('flag', 15),
    'flag0_lb': ('flag', 0),
    'flag1_lb': ('flag', 1),
    'flag2_lb': ('flag', 2),
    'flag3_lb': ('flag', 3),
    'flag4_lb': ('flag', 4),
    'flag5_lb': ('flag', 5),
    'flag6_lb': ('flag', 6),
    'flag7_lb': ('flag', 7),
    'flag0_hb': ('flag', 8),
    'flag1_hb': ('flag', 9),
    'flag2_hb': ('flag', 10),
    'flag3_hb': ('flag', 11),
    'flag4_hb': ('flag', 12),
    'flag5_hb': ('flag', 13),
    'flag6_hb': ('flag', 14),
    'flag7_hb': ('flag', 15),
    's8_lb': ('s8', 0),
    's8_hb': ('s8', 8),
    'u8_lb': ('u8', 0),
    'u8_hb': ('u8', 8),
    's16': ('s16',),
    'u16': ('u16',),
    'q7_8': ('q7_8',),
}


@cv.schema_extractor("callback")
def callback(value):
    one_of_validator = cv.one_of(*CALLBACK_ALIASES, lower=True)
    if value == cv.SCHEMA_EXTRACT:
        return one_of_validator

    value = one_of_validator(value)
    return CALLBACK_ALIASES[value]


CALLBACK_POSITION_ALIASES = {
    'flag': ('flag',),
    's8': ('s8',),
    'u8': ('u8',),
}

CALLBACK_POSITION_SCHEMA = cv.Schema({
    cv.Required(CONF_CALLBACK): cv.one_of(*CALLBACK_POSITION_ALIASES, lower=True),
    cv.Required(CONF_POSITION): cv.int_,
})


@cv.schema_extractor("callback_position")
def callback_position(value):
    if value == cv.SCHEMA_EXTRACT:
        return CALLBACK_POSITION_SCHEMA

    config = CALLBACK_POSITION_SCHEMA(value)
    return *CALLBACK_POSITION_ALIASES[config[CONF_CALLBACK]], config[CONF_POSITION]


CALLBACK_MASK_SCHEMA = cv.Schema({
    cv.Required(CONF_POSITION): cv.int_,
    cv.Required(CONF_LENGTH): cv.int_,
})


@cv.schema_extractor("callback_mask")
def callback_mask(value):
    if value == cv.SCHEMA_EXTRACT:
        return CALLBACK_MASK_SCHEMA

    config = CALLBACK_MASK_SCHEMA(value)
    return 'mask', config[CONF_LENGTH], config[CONF_POSITION]


CALLBACK_SCHEMA = cv.Any(
    callback,
    callback_position,
    callback_mask
)


def opentherm_component_schema(is_write: bool) -> cv.Schema:
    message_type_default: str = "WRITE_DATA" if is_write else "READ_DATA"
    return cv.Schema({
        cv.GenerateID(CONF_OPENTHERM_ID): cv.use_id(OpenthermHub),
        cv.Required(CONF_MESSAGE_ID): cv.Any(cv.enum(OPENTHERM_MESSAGE_ID, upper=True), cv.uint8_t),
        cv.Optional(CONF_MESSAGE_TYPE, default=message_type_default): cv.enum(OPENTHERM_MESSAGE_TYPE, upper=True),
        cv.Optional(CONF_READ_CALLBACK): CALLBACK_SCHEMA,
        cv.Optional(CONF_WRITE_CALLBACK): CALLBACK_SCHEMA,
    })


def get_callback(method: str, *template_args):
    cb: MockObj = getattr(_static_OpenthermData, method)
    method = MockObj(method)
    if template_args:
        cb = cb.template(*template_args)
        method = method.template(*template_args)

    return cg.RawExpression(f'&{cb}'), str(method)


async def register_component(var, config):
    await cg.register_parented(var, config[CONF_OPENTHERM_ID])

    cg.add(var.set_message_id(config[CONF_MESSAGE_ID]))
    cg.add(var.set_message_type(config[CONF_MESSAGE_TYPE]))

    if args := config.get(CONF_READ_CALLBACK):
        cb, name = get_callback(*args)
        cg.add(var.set_read_callback(cb, name))

    if args := config.get(CONF_WRITE_CALLBACK):
        cb, name = get_callback(*args)
        cg.add(var.set_write_callback(cb, name))


async def to_code(config: dict) -> None:
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    transmitter = await cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_transmitter(transmitter))

    receiver = await cg.get_variable(config[CONF_RECEIVER_ID])
    cg.add(var.set_receiver(receiver))
