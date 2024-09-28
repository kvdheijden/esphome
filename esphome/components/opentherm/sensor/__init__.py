import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, opentherm
from esphome.components.opentherm import opentherm_ns, OpenthermFloatComponent
from esphome.const import CONF_ID

OpenthermSensor = opentherm_ns.class_(
    "OpenthermSensor", OpenthermFloatComponent, sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        OpenthermSensor,
    )
    .extend(opentherm.opentherm_component_schema(False))
    .extend(cv.polling_component_schema("30s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    await opentherm.register_component(var, config)
