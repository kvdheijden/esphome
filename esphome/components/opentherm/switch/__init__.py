import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, opentherm
from esphome.components.opentherm import opentherm_ns, OpenthermBinaryComponent
from esphome.const import CONF_ID

OpenthermSwitch = opentherm_ns.class_(
    "OpenthermSwitch", OpenthermBinaryComponent, switch.Switch, cg.Component
)

CONFIG_SCHEMA = cv.All(
    switch.switch_schema(
        OpenthermSwitch,
    )
    .extend(opentherm.opentherm_component_schema(True))
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await switch.register_switch(var, config)
    await opentherm.register_component(var, config)
