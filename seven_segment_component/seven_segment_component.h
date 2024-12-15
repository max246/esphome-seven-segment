#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/time.h"

#include <vector>

namespace esphome {
namespace seven_segment_component {

class SevenSegmentComponent;
using seven_segment_writer_t = std::function<void(SevenSegmentComponent &)>;


class SevenSegmentComponent : public PollingComponent {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  
  void post_number(uint8_t number, bool decimal);
  void show_number(float value);
  void print(const char *str);
  /// Evaluate the printf-format and print the result at position 0.
  uint8_t printf(const char *format, ...) __attribute__((format(printf, 2, 3)));
  
  
  void set_writer(seven_segment_writer_t &&writer) { this->writer_ = writer; }

  void set_clk_pin(GPIOPin *pin) { clk_pin_ = pin; }
  void set_data_pin(GPIOPin *pin) { data_pin_ = pin; }
  void set_latch_pin(GPIOPin *pin) { latch_pin_ = pin; }
  
 protected:
  GPIOPin *data_pin_;
  GPIOPin *clk_pin_;
  GPIOPin *latch_pin_;
  
  optional<seven_segment_writer_t> writer_{};

};


}  // namespace seven_segment_component
}  // namespace esphome
