import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome import pins

from esphome.const import (
    CONF_ID,
    CONF_CLK_PIN,
    CONF_DATA_PIN,
    CONF_WRITE_PIN,
    CONF_LAMBDA
)
    

seven_segment_component_ns = cg.esphome_ns.namespace('seven_segment_component')
SevenSegmentComponent = seven_segment_component_ns.class_('SevenSegmentComponent', cg.PollingComponent)
SevenSegmentComponentRef = SevenSegmentComponent.operator("ref")


CONFIG_SCHEMA = display.BASIC_DISPLAY_SCHEMA.extend(
    {
    cv.GenerateID(): cv.declare_id(SevenSegmentComponent),
    cv.Required(CONF_CLK_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_DATA_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_WRITE_PIN): pins.gpio_output_pin_schema,
}
).extend(cv.polling_component_schema("1s"))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    
    clk = await cg.gpio_pin_expression(config[CONF_CLK_PIN])
    cg.add(var.set_clk_pin(clk))
    data = await cg.gpio_pin_expression(config[CONF_DATA_PIN])
    cg.add(var.set_data_pin(data))
    latch = await cg.gpio_pin_expression(config[CONF_WRITE_PIN])
    cg.add(var.set_latch_pin(latch))
    
    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(SevenSegmentComponentRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
