#include "seven_segment_component.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace seven_segment_component {

     //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

#define a  1<<0
#define b  1<<6
#define c  1<<5
#define d  1<<4
#define e  1<<3
#define f  1<<1
#define g  1<<2
#define dp 1<<7

static const char *TAG = "seven_segment_component.component";

void SevenSegmentComponent::setup() {

  this->clk_pin_->setup();               // OUTPUT
  this->clk_pin_->digital_write(false);  // LOW
  this->data_pin_->setup();               // OUTPUT
  this->data_pin_->digital_write(false);  // LOW
  this->latch_pin_->setup();               // OUTPUT
  this->latch_pin_->digital_write(false);  // LOW
  

}


void SevenSegmentComponent::update() {
  if (this->writer_.has_value())
    (*this->writer_)(*this);
}

void SevenSegmentComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Seven Segment component");

  LOG_PIN("  CLK Pin: ", this->clk_pin_);
  LOG_PIN("  DATA Pin: ", this->data_pin_);
  LOG_PIN("  Latch Pin: ", this->latch_pin_);
  LOG_UPDATE_INTERVAL(this);
}

float SevenSegmentComponent::get_setup_priority() const { return setup_priority::PROCESSOR; }

uint8_t SevenSegmentComponent::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0) {
     this->print(buffer);
     return 1;
  }
  return 0;
}


void SevenSegmentComponent::print(const char *str) {
    // ESP_LOGD(TAG, "Calling print");
    // ESP_LOGD(TAG, "[%s]  Data coming in",  str);

    uint32_t len = 0;
    for (uint32_t z = 0;; z++) {
        if (str[z] == '\0')
            break;
        len += 1;
    }
    
    for (uint32_t i = len-1;; i--) {
         uint8_t number = str[i];
         if (number >= 48 && number <= 57) {
             uint8_t real_number = str[i] - '0';
             ESP_LOGD(TAG, "[%d]  number parsing ", real_number);
             this->post_number(real_number, false);
         } else {
            ESP_LOGD(TAG, "[%c]  char parsing ",  str[i]);
            this->post_number(number, false);
         }
         if (i <= 0) break;
      }
    //Latch the current segment data
    this->latch_pin_->digital_write(false);
    this->latch_pin_->digital_write(true); //Register moves storage register on the rising edge of RCK
}

void SevenSegmentComponent::post_number(uint8_t number, bool decimal) {
    uint8_t segments = 0;

    switch (number)
    {
        case 1: segments = b | c; break;
        case 2: segments = a | b | d | e | g; break;
        case 3: segments = a | b | c | d | g; break;
        case 4: segments = f | g | b | c; break;
        case 5: segments = a | f | g | c | d; break;
        case 6: segments = a | f | g | e | c | d; break;
        case 7: segments = a | b | c; break;
        case 8: segments = a | b | c | d | e | f | g; break;
        case 9: segments = a | b | c | d | f | g; break;
        case 0: segments = a | b | c | d | e | f; break;
        case ' ': segments = 0; break;
        case 'c': segments = g | e | d; break;
        case '-': segments = g; break;
    }
    
    if (decimal) segments |= dp;
    
    //Clock these bits out to the drivers
    for (uint8_t x = 0 ; x < 8 ; x++)
    {
        this->clk_pin_->digital_write(false); 
        this->data_pin_->digital_write(segments & 1 << (7 - x));  
        this->clk_pin_->digital_write(true); //Data transfers to the register on the rising edge of SRCK
    }
  }

void SevenSegmentComponent::show_number(float value) {
    int number = abs(value); //Remove negative signs and any decimals
    
    
    for (uint8_t x = 0 ; x < 2 ; x++)
    {
    int remainder = number % 10;
    
    this->post_number(remainder, false);
    
    number /= 10;
    }
    
    //Latch the current segment data
    this->latch_pin_->digital_write(false);
    this->latch_pin_->digital_write(true); //Register moves storage register on the rising edge of RCK

}

}  // namespace seven_segment_component
}  // namespace esphome
