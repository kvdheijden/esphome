import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, opentherm
from esphome.components.opentherm import opentherm_ns, OpenthermBinaryComponent
from esphome.const import CONF_ID

OpenthermBinarySensor = opentherm_ns.class_(
    "OpenthermBinarySensor", OpenthermBinaryComponent, binary_sensor.BinarySensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(
        OpenthermBinarySensor,
    )
    .extend(opentherm.opentherm_component_schema(False))
    .extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await binary_sensor.register_binary_sensor(var, config)
    await opentherm.register_component(var, config)
