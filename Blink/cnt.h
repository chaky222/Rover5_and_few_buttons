#ifndef cnt_h
#define cnt_h

#include <Arduino.h>
// #define 

typedef struct {
  uint16_t interval = 0;
  uint8_t next_item = 255;
  // uint16_t tmp = 111;
  // uint8_t priority; // maybe this not need...
  // void* inter();
  void (*functionPtr)();
} interrupts_stek_item;

class cnt_class{
  public:    
    cnt_class();
    uint16_t INTERRUPT_COUNTS = 65536;
    interrupts_stek_item * ints = new interrupts_stek_item [6];
    uint8_t current_item = 255;
    void set_time(uint16_t tacts, uint8_t index);
    void launch_timers();

};






#endif

