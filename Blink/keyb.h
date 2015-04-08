#ifndef keyb_h
#define keyb_h


#include <Arduino.h>


class Tkey{
  public: 
    Tkey();
    void init (uint8_t in_pin, bool in_adc);
    bool Tkey_enabled = false;
    bool adc = false;
    uint8_t pin = 0, value=0;
    volatile uint8_t old_value=0; 
    uint8_t have_changes();
    uint8_t read_value();
};




class keyb_class{
  public:  
    keyb_class(uint8_t p_PWM_ADJUST,uint8_t p_TRAVEL_ADJUST,uint8_t p_forward,uint8_t p_backward,uint8_t p_stop,uint8_t p_learn);  
    // keyb_class(uint8_t p_ADC1, uint8_t p_ADC2, uint8_t p_init, uint8_t p_stop);
    void inter();
    void check_keys_changes();    
    uint16_t calc_keyb = 0;    
    Tkey * keys[6] = {new Tkey(),new Tkey(),new Tkey(),new Tkey(),new Tkey(),new Tkey()};
    // void (*set_time)(uint16_t tacts, uint8_t index);
    // void onKey_change_callBack(uint8_t key_pin,uint8_t new_val, bool is_adc){  
    void (*onKey_change)(uint8_t key_pin,uint8_t new_val, bool is_adc);
    // void (*onKey_change1)(uint8_t key_pin,uint8_t new_val,uint8_t is_adc);    
};






#endif

