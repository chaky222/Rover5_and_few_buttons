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
  cli();
  
    // TCNT1=65520;
    for (uint8_t u = 0;u<4;u++){ p->motor[u]->new_pos = p->motor[u]->e->read(); }
    p->calc1++;
    TCNT1=65533;
    
    // timer_div();
  
  sei();
//  return;
  // p->inter();
  // p->calc1++;
  // TCNT1 = 10000;// 65536
  //TCNT2 = 0;
  // calc1=TCNT1;
}

uint16_t timer_calc = 0;

uint16_t proc[3][2]{  // 1-st count, 2-nd - divider  16kHZ will div to this:
                      {0,2},//500 Hz
                      {0,64},// 16Hz
                      {0,1024} // 1Hz  
                      // {0,16},//1 kHz
                      // {0,1024},// 16Hz
                      // {0,16384} // 1Hz                     
                    };

ISR(TIMER2_OVF_vect) {
  sei();
  timer_div();
  // TCNT2 = 192;
  // TCNT2 = 240;
  // p->calc1++;
  
  // for (uint8_t u = 0;u<4;u++){ p->motor[u]->new_pos = p->motor[u]->e->read(); }
  

}
void timer_div(){
  timer_calc++;
  for (uint8_t u =0;u<3;u++){
    uint16_t proc3_count_tmp = (timer_calc & proc[u][1]);
    if (!(proc3_count_tmp == proc[u][0])){
      proc[u][0] = proc3_count_tmp;
      switch(u){
        case 0:// 1kHz
          
          p->motor[0]->speed_control_interrupt();
          p->motor[1]->speed_control_interrupt();
          p->motor[2]->speed_control_interrupt();
          p->motor[3]->speed_control_interrupt();
          p->tim2++; 
          break; 
        case 1: // 16Hz
          p->keyb->inter();  
          // p->calc1++;
          break;
        case 2: // 1Hz
          p->plan_inter(); 
          // p->calc1++;
          break; 
        default: 
        break;
      } 
      // 
    }

  } 

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
  // p->calc1++; 
  p->motor[0]->speed_control_interrupt();
  p->motor[1]->speed_control_interrupt();
  p->motor[2]->speed_control_interrupt();
  p->motor[3]->speed_control_interrupt();
  p->cnt->set_time(96,2);//  1kHz
  
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
  // delay(4000); // dalay for normal launch timers!!! interrupts - is evil! :)
  p->load_from_eeprom();
  digitalWrite(13,1);
  p->cnt->launch_timers();
  plan_loaded = true;
  // p->go_all_home_pos();
  

  
}



void loop() {
  p->cmd_loop();
  // delay(1000);
  // p->send_debug_info();

}
