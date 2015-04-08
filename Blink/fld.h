#ifndef fld_h
#define fld_h

#include <Arduino.h>


class fld_class{
  public:    
    fld_class();
    void inter();
    uint16_t calc_1 = 0;
    void (*set_time)(uint16_t tacts, uint8_t index);
    // void* set_time(uint16_t tacts, uint8_t index);

};






#endif

