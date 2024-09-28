import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number, opentherm
from esphome.components.opentherm import opentherm_ns, OpenthermFloatComponent
from esphome.const import CONF_ID, CONF_MIN_VALUE, CONF_MAX_VALUE, CONF_STEP

OpenthermNumber = opentherm_ns.class_(
    "OpenthermNumber", OpenthermFloatComponent, number.Number, cg.Component
)

CONFIG_SCHEMA = cv.All(
    number.number_schema(OpenthermNumber)
    .extend({
        cv.Required(CONF_MIN_VALUE): cv.float_,
        cv.Required(CONF_MAX_VALUE): cv.float_,
        cv.Optional(CONF_STEP, default=0.00390625): cv.float_,
    })
    .extend(opentherm.opentherm_component_schema(True))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await number.register_number(var,
                                 config,
                                 min_value=config[CONF_MIN_VALUE],
                                 max_value=config[CONF_MAX_VALUE],
                                 step=config[CONF_STEP])
    await opentherm.register_component(var, config)
