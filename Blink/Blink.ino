// #include <Scheduler.h>
// #define INPUT_PULLUP 
#include <avr/interrupt.h>
#include <avr/io.h>

#include "plan.h"

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
uint16_t calc2 = 0;
uint16_t calc1 = 0;

// volatile bool all_loops_not_launched = true;
// volatile uint8_t next_launch = 0;


 plan_class * p;// = new plan_class();
 volatile bool plan_loaded = false;

ISR(TIMER1_OVF_vect) {
//  return;
  p->inter();
  // p->calc1++;
  // TCNT1 = 10000;// 65536
  //TCNT2 = 0;
  // calc1=TCNT1;
}
 
ISR(TIMER2_OVF_vect) {
  // p->tim2++;   
  // for (uint8_t u = 0;u<4;u++){ 
  //   p->motor[u]->e->read();
  // }
  // TCNT2 = 240; // 8kHz encoders read. timer2 have more priority.
   
}

void plan_callBack(){ 
  p->cnt->set_time(50000,0);  // 2 Hz
  p->plan_inter(); 
  
}
void fld_callBack(){   
  // p->cnt->set_time(500,1); // 100 hz
  // p->fld->inter(); 
}

void motors_callBack(){  
  p->motor[0]->speed_control_interrupt();
  p->motor[1]->speed_control_interrupt();
  p->motor[2]->speed_control_interrupt();
  p->motor[3]->speed_control_interrupt();
  p->cnt->set_time(6,2);//  4kHz
  // p->calc1++;
}
void keyb_callBack(){  
  p->cnt->set_time(3200,3); // 10 Hz
  p->keyb->inter();  
}


void onKey_change_callBack(uint8_t key_pin,uint8_t new_val, bool is_adc){  
  // calc1++;
  if (plan_loaded){
    p->onKey_change(key_pin,new_val,is_adc);
  }
}
// void onKey_change_callBack1(uint8_t key_pin,uint8_t new_val,uint8_t is_adc){  
//   calc1++;
//   // p->onKey_change(key_pin,new_val,is_adc);
// }

void set_time_callBack(uint16_t tacts,uint8_t index){ p->cnt->set_time(tacts,index);}


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  // Scheduler.startLoop(loop2);
  
  Serial.begin(115200);
  Serial.println("Analyze IR Remote");
  p = new plan_class();
  p->fld->set_time = &set_time_callBack;
  p->cnt->ints[0].functionPtr =  &plan_callBack;
  p->cnt->ints[1].functionPtr =  &fld_callBack;
  p->cnt->ints[2].functionPtr =  &motors_callBack;
  p->cnt->ints[3].functionPtr =  &keyb_callBack;  
  p->keyb->onKey_change       =  &onKey_change_callBack;  
  // p->keyb->onKey_change1      =  &onKey_change_callBack1;  
  // p->keyb->set_time = &set_time_callBack;
  p->cnt->set_time(5000,0); // 10 hz
  p->cnt->set_time(500,1); // 100 hz
  p->cnt->set_time(50,2); 
  p->cnt->set_time(1000,3); // 100 Hz
  digitalWrite(13,0);
  delay(4000); // dalay for normal launch timers!!! interrupts - is evil! :)
  p->load_from_eeprom();
  digitalWrite(13,1);
  p->cnt->launch_timers();
  plan_loaded = true;
  // p->go_all_home_pos();
  

  
}



void loop() {
  p->cmd_loop();

}
