/*  
Dagu4Motor.cpp - Library for driving the Dagu4Motor Driver code.  
Created by William Garrdio
Created on: 02/03/2012
02/25/2012 - Rewrite class to handle 1 motor per instance
11/24/2013 - Removed Teensy Encoder library, better if called from Sketch

*/  
#include "Arduino.h"  
#include "Dagu4Motor.h"

inline int32_t pcm_u8_to_s32(uint8_t n){ return (n << 24) ^ 0x80000000; }
inline int32_t pcm_u8_to_s16(uint8_t n){ return (n << 8) ^ 0x8000;  }


Dagu4Motor::Dagu4Motor(uint8_t pwmPin, uint8_t dirPin, uint8_t currPin,uint8_t enc_pin1, uint8_t enc_pin2)
{  
    e = new Encoder(enc_pin1,enc_pin2);
    _pwmPin = pwmPin;  
    _dirPin = dirPin;  
    _currPin = currPin;
    pinMode(_pwmPin, OUTPUT);  
    pinMode(_dirPin, OUTPUT); 
    pinMode(_currPin, INPUT);  
    // analogWrite(_pwmPin , 255);
    // digitalWrite(_dirPin,1);
}


void Dagu4Motor::motor_take_dest(){
  if (free_cmd_on_take_dest){    
    free_motor_from_cmd(0,0,true);
    // motor_took_dest = true;
  }
}

void Dagu4Motor::motor_stay_too_long(){
  // if (_pwmSpeed>0){
    // max_pwm_power = 0;
    // blocked_move_interrupts = 0;
    motor_is_blocked_now = true;
    // setSpeed(0);
  // }
}

void Dagu4Motor::speed_control_interrupt(){
    int32_t new_position = e->read();  
    // int32_t new_position = e->encoder.position;
    if ((max_pwm_power>0) ){      
      uint8_t new_pwm = 0;
      // bool new_dir = false;  
      if (!(new_position == dest_position)){//if (new_position != dest_position){
        if (!motor_is_blocked_now){
          blocked_move_interrupts++;         
          int32_t new_stay_delta = abs(new_position - dest_position);

          if (new_stay_delta < curr_blocked_move_interrupt_stay_delta) curr_blocked_move_interrupt_stay_delta = new_stay_delta;
          if (blocked_move_interrupts > max_count_without_move_ms){
            blocked_move_interrupts = 0;
            // debug_tmp = curr_blocked_move_interrupt_stay_delta;
            if ((curr_blocked_move_interrupt_stay_delta >= last_blocked_move_interrupt_stay_delta)) {  
              if ((curr_blocked_move_interrupt_stay_delta <= MOTOR_STAY_DELTA ) && (last_blocked_move_interrupt_stay_delta <= MOTOR_STAY_DELTA )){
                motor_took_dest = true;
              }
              motor_stay_too_long();
              // return ;
            }
            last_blocked_move_interrupt_stay_delta = curr_blocked_move_interrupt_stay_delta;
            curr_blocked_move_interrupt_stay_delta = 200000;          
          }
        }else{  
          if (_pwmSpeed>0){    
            blocked_move_interrupts++;   
            if (blocked_move_interrupts > max_count_die_pwm_interupts){            
              blocked_move_interrupts = 0;
              error_code = 2; //  Motor can not run! here an error!!! Motor stay and can not shut down his pwm output! warning situation - shutdown pwm!warning situation - shutdown pwm!
              max_pwm_power=0;
              free_motor_from_cmd(0,0,true);
            }
          }else{
            blocked_move_interrupts = 0;
          }
        }
        new_pwm = max_pwm_power;
        // new_dir = (new_position < dest_position);        
      }else{
        // blocked_move_interrupts = 0;
        motor_take_dest();
      }
      setSpeed(new_pwm,0,0,false,true);
      setMotorDirection(new_position < dest_position);
    // }
    }

    position = new_position;
  // }   
}

// void Dagu4Motor::recalc_speed_from_encoder(int32_t *new_position)
// {

//   // position == &new_position;
//   // without_move_ms=0;
// }



// void Dagu4Motor::stopMotor()  
// {  
//     setSpeed(0);  
// }  


volatile void Dagu4Motor::free_motor_from_cmd(uint8_t cmd_num,uint8_t cmd_index, bool ignore_cmd_access){
  if ((ignore_cmd_access) || ((cmd_num == cmd_num_current) && (cmd_index==cmd_index_current)) ){
    free_cmd_on_take_dest=false;
    cmd_num_current = 0;
    get_access(0);
  }
}

volatile uint8_t Dagu4Motor::get_access(uint8_t cmd_num){
  if (cmd_num_current == 0){
    cmd_num_current = cmd_num;
    cmd_index_current++;
    if (cmd_index_current==0) cmd_index_current++;
    return cmd_index_current;
  }
  return 0;
}
volatile void Dagu4Motor::set_dest_position(int32_t new_dest_position, uint8_t cmd_num,uint8_t cmd_index, bool try_take_access,bool in_free_cmd_on_take_dest, bool ignore_cmd_access){
  if ((ignore_cmd_access) || (check_cmd_access(cmd_num,cmd_index, try_take_access))) {
    if (in_free_cmd_on_take_dest){  free_cmd_on_take_dest=in_free_cmd_on_take_dest;  } 
    dest_position = new_dest_position;
  }
}

volatile bool Dagu4Motor::check_cmd_access(uint8_t cmd_num,uint8_t cmd_index, bool try_take_access){
  // bool result = false;
  if ((cmd_num == cmd_num_current) && (cmd_index==cmd_index_current) ){
    return true;
  }else{
    if (try_take_access){
      if (get_access(cmd_num)>0){
        return true;
      }
    }
  }
  return false;
}


volatile uint8_t Dagu4Motor::setSpeed(uint8_t speedMotor,uint8_t cmd_num,uint8_t cmd_index, bool try_take_access, bool ignore_cmd_access,bool in_free_cmd_on_take_dest ) {  
  if ((ignore_cmd_access) || (check_cmd_access(cmd_num,cmd_index, try_take_access))) {
   // if ((false) || (false)) {
    if ((!(speedMotor == _pwmSpeed)) ) {  
      if (in_free_cmd_on_take_dest){  free_cmd_on_take_dest=in_free_cmd_on_take_dest;  }      
      // debug_tmp = debug_tmp+  100;       
      is_run_now =  (speedMotor>0);    
      if (speedMotor>0){
        motor_is_blocked_now = false;
        motor_took_dest = false;
        // debug_tmp = debug_tmp+ 1;      
        blocked_move_interrupts = 0;       
        last_blocked_move_interrupt_stay_delta = 200000; 
        curr_blocked_move_interrupt_stay_delta = 200000;   
        max_pwm_power = speedMotor;      
      }
      analogWrite(_pwmPin, speedMotor);
      _pwmSpeed=speedMotor;
    }
    return cmd_index_current;
  }
  return 0;
}

// void Dagu4Motor::setSpeed_with_dir(int16_t speedMotor)  
// {  
//   setMotorDirection(speedMotor>0);
//   speedMotor = abs(speedMotor);
//   speedMotor = constrain(speedMotor, 0, 255);
//   setSpeed(speedMotor);
// }

void Dagu4Motor::setMotorDirection(bool isMotor)  
{ 
  if (isMotor != motor_dir){
    motor_dir = isMotor;
    if(isMotor) {  
      digitalWrite(_dirPin, !invert_direction);  //Reverse motor direction, 1 high, 2 low  
    } else {       
      digitalWrite(_dirPin, invert_direction);  //Set motor direction, 1 low, 2 high  
    }
  }
}

uint16_t Dagu4Motor::getCurrent()
{
    // _curr_last_Readed = analogRead(_currPin);
    return  analogRead(_currPin);
}

// uint8_t Dagu4Motor::getSpeed()
// {  
//     return Speed;
// }






    
