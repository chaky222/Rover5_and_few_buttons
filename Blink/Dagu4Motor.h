/*  
*************************************************************************************
Dagu4Motor.h - Library for driving the Dagu4Motor Driver code.  
Created by William Garrdio 
Edited by chaky22222222@gmail.com 2015
*************************************************************************************
Notes
pwmPin: Digital pin to set motor speed
dirPin: Digital pin to set motor direction
currPin: Analog in pin to monitor current usage
enc_pin1: Digital pin for encoder A, 
enc_pin2: Digital pin for encoder B
*************************************************************************************
*/
#ifndef Dagu4Motor_h  
#define Dagu4Motor_h
#define ENCODER_DO_NOT_USE_INTERRUPTS

#define cmd_STATUS_FREE         0
#define cmd_STATUS_CANCELED     1
#define cmd_STATUS_ERROR        2
#define cmd_STATUS_NORMAL_DONE  5
#define cmd_STATUS_PAUSED       6
#define cmd_STATUS_RUNING       10



#define MOTOR_STAY_DELTA        10 //dest_position+- stay_delta = motor_took_dest

#include "Encoder.h" 
//#define ENCODER_OPTIMIZE_INTERRUPTS //Fails to compile, vector errors

class Dagu4Motor  
{  
public:  
    Encoder *e;
    uint8_t _pwmPin=0,_pwmPin2=0,_dirPin=0,_currPin=0;
    Dagu4Motor(uint8_t pwmPin,uint8_t pwmPin2, uint8_t dirPin, uint8_t currPin,uint8_t enc_pin1, uint8_t enc_pin2); 

    	 
    uint8_t setSpeed (uint8_t speedMotor,uint8_t cmd_num,uint8_t cmd_index, bool try_take_access, bool ignore_cmd_access = false,bool in_free_cmd_on_take_dest = false, bool update_max_speed = true);  
    uint8_t setSpeed1(uint8_t speedMotor,uint8_t cmd_num,uint8_t cmd_index, bool try_take_access, bool ignore_cmd_access = false,bool in_free_cmd_on_take_dest = false, bool update_max_speed = true);  
        // void setSpeed_with_dir(int16_t speedMotor);  
    volatile uint8_t get_access(uint8_t cmd_index);
    volatile bool check_cmd_access(uint8_t cmd_num,uint8_t cmd_index, bool try_take_access);	
    volatile void free_motor_from_cmd(uint8_t cmd_num,uint8_t cmd_index, bool ignore_cmd_access = false);
    volatile void set_dest_position(int32_t new_dest_position, uint8_t cmd_num,uint8_t cmd_index, bool try_take_access,bool in_free_cmd_on_take_dest = false, bool ignore_cmd_access = false);
      
	uint16_t getCurrent();
	// uint8_t getSpeed();


    void speed_control_interrupt();
    void speed_control_interrupt1();
    void motor_stay_too_long();
    void motor_take_dest();



    uint8_t cmd_num_current = 0,cmd_index_current = 1, cmd_status = 0, free_cmd_on_take_dest = false;// 0=free, not buzy. 1=canceled, 2=error,  5=normal_done, 6=PAUSED 10=buzy now,

    uint8_t  min_pwm_power = 5;
    uint8_t max_delta_pos_between_speed_control = 7; // 32/1 = encoders_reads/speed_controls_interrupts, so max speed=8. I took 7, but my encoder at home can reading on 2 only :( .
    uint8_t max_pwm_power = 254;
    uint8_t global_motor_max_pwm_power = 254;
    int32_t position = 0;
    int32_t new_pos = 0;
    int32_t dest_position = 0;
    int32_t max_position = 100000;
    // uint32_t Speed;

    // volatile int32_t current_speed;
    bool is_run_now = false,  init_done=false,init_run=false;
    bool invert_direction = false;
    bool motor_took_dest = false; 
	bool motor_dir = false/*0=minus_pos, 1=plus_pos*/, motor_is_blocked_now = false;

    
    uint16_t blocked_move_interrupts = 0;
    int32_t last_blocked_move_interrupt_stay_delta = 200000;
    int32_t curr_blocked_move_interrupt_stay_delta = 200000;
    
    

    uint16_t max_count_die_pwm_interupts = 4000;
    uint16_t max_count_without_move_ms = 2000; // 8000*2
	int32_t debug_tmp = 0;
    uint16_t error_code = 0;
private:  
    void stopMotor(); 
    void setMotorDirection (bool isMotor);  
    void setMotorDirection1(bool isMotor);  
    
   
    volatile uint8_t _pwmSpeed=0;  
    // void recalc_speed_from_encoder(int32_t *new_position);
    int16_t maxSpeed_min_interrupts_beetween_1_encoder_imp = 10; // if less this value - maxspeed more faster!!!! 8000=8kHz
    //formula: maxspeed = (8000/maxSpeed_min_interrupts_beetween_1_encoder_imp)  => 8000/10 = 800 imps per second, 8000/2 = 4000 imps per second
    int16_t current_interrupts_beetween_1_encoder_imp = 4000;

    
    // uint16_t _curr_last_Readed = 0;
    // uint16_t _currRate;
    // uint8_t _speed;

};


#endif
