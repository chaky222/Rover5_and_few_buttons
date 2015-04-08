
#include "keyb.h"
#include <avr/interrupt.h>
#include <avr/io.h>

Tkey::Tkey(){}
void Tkey::init(uint8_t in_pin, bool in_adc){ 
  Tkey_enabled = (in_pin>0) ? true : false;
  // Tkey_enabled = (in_pin>0);
  if (Tkey_enabled){
    pin = in_pin;
    adc = in_adc;
    // pinMode(pin, INPUT);  
    if (in_adc){
      pinMode(pin, INPUT); 
    }else{
      pinMode(pin, INPUT_PULLUP);  
    }
    have_changes();
  }
}
uint8_t Tkey::read_value(){
  if (Tkey_enabled){
    if (adc){
      // if (need_ADC_check){
        return analogRead(pin)/4;
      // }else{
        // return value;
      // }
    }else{
      return digitalRead(pin);
    }
  }
  return 0;
}

uint8_t Tkey::have_changes(){
  // bool result = false;

  value =  read_value();
  if (!(value == old_value)){
    old_value = value ;
    return 1; 
  }    
  return 0;
}

void keyb_class::inter(){  
  check_keys_changes();
  calc_keyb++;  
}
keyb_class::keyb_class(uint8_t p_PWM_ADJUST,uint8_t p_TRAVEL_ADJUST,uint8_t p_forward,uint8_t p_backward,uint8_t p_stop,uint8_t p_learn){
    keys[0]->init(p_PWM_ADJUST,     true); 
    keys[1]->init(p_TRAVEL_ADJUST,  true); 
    keys[2]->init(p_forward,        false); 
    keys[3]->init(p_backward,       false);    
    keys[4]->init(p_stop,           false); 
    keys[5]->init(p_learn,          false); 
}

void keyb_class::check_keys_changes(){
  // onKey_change(1,0,false);
  // onKey_change(1,0,0);

  for (uint8_t u = 0;u<6;u++){   
    if (keys[u]->have_changes()>0){
      onKey_change(keys[u]->pin, keys[u]->value, keys[u]->adc);
      // onKey_change(1, 0, false);
      // onKey_change(1,0,0);
      // delay(100);
    }
  }
}



