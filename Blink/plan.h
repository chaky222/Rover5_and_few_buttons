#ifndef plan_h
#define plan_h

#include <Arduino.h>
#include "cnt.h"
#include "Configuration.h"
#include "language.h"
#include "fld.h"
#include "Dagu4Motor.h"
#include "keyb.h"

#define VK_KEY_DOWN   1
#define VK_KEY_UP     0


  static void serialprintPGM(const char *str) {
    char ch=pgm_read_byte(str);
    while(ch)
    {
      Serial.write(ch);
      ch=pgm_read_byte(++str);
    }
  }


  #define SERIAL_PROTOCOL(x) (Serial.print(x))
  #define SERIAL_PROTOCOL_F(x,y) (Serial.print(x,y))
  #define SERIAL_PROTOCOLPGM(x) (serialprintPGM(PSTR(x)))
  #define SERIAL_PROTOCOLLN(x) (Serial.print(x),Serial.write('\n'))
  #define SERIAL_PROTOCOLLNPGM(x) (serialprintPGM(PSTR(x)),Serial.write('\n'))


  const char errormagic[] PROGMEM ="Error:";
  const char echomagic[] PROGMEM ="echo:";
  #define SERIAL_ERROR_START (serialprintPGM(errormagic))
  #define SERIAL_ERROR(x) SERIAL_PROTOCOL(x)
  #define SERIAL_ERRORPGM(x) SERIAL_PROTOCOLPGM(x)
  #define SERIAL_ERRORLN(x) SERIAL_PROTOCOLLN(x)
  #define SERIAL_ERRORLNPGM(x) SERIAL_PROTOCOLLNPGM(x)

  #define SERIAL_ECHO_START (serialprintPGM(echomagic))
  #define SERIAL_ECHO(x) SERIAL_PROTOCOL(x)
  #define SERIAL_ECHOPGM(x) SERIAL_PROTOCOLPGM(x)
  #define SERIAL_ECHOLN(x) SERIAL_PROTOCOLLN(x)
  #define SERIAL_ECHOLNPGM(x) SERIAL_PROTOCOLLNPGM(x)

  #define SERIAL_ECHOPAIR(name,value) (serial_echopair_P(PSTR(name),(value)))

  static void serial_echopair_P(const char *s_P, float v)          { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, double v)         { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, unsigned long v)  { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, int32_t v)        { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, int16_t v)        { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, int8_t v)         { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, uint16_t v)       { serialprintPGM(s_P); SERIAL_ECHO(v); }
  static void serial_echopair_P(const char *s_P, bool v)           { serialprintPGM(s_P); if (v){ SERIAL_ECHO("1"); }else SERIAL_ECHO("0"); }
     
      // MAX PWM_ADJUST   A5    
      // TRAVEL_ADJUST    A4 
      // FORWARD          30 
      // BACKWARD         31 
      // STOP             32 
      // LEARN            33 
  // static const uint8_t p_ADC1 = 10, p_ADC2 = 11, p_init = 26, p_stop = 28;  // p_ADC1 = pwm , p_ADC2 = distance in persents
  static const uint8_t p_PWM_ADJUST = 0, p_TRAVEL_ADJUST = A4,p_forward = 30, p_backward = 31,  p_stop = 32, p_learn = 33;  // p_ADC1 = pwm , p_ADC2 = distance in persents
  // static const uint8_t p_PWM_ADJUST = 0, p_TRAVEL_ADJUST = A4,p_forward = 30, p_backward = 32,  p_stop = 34, p_learn = 36;  // p_ADC1 = pwm , p_ADC2 = distance in persents


// Initialize the motion plan
class plan_class{
  public:    
    // bool all_loaded = false;

    
    cnt_class   * cnt   = new cnt_class();
    fld_class   * fld   = new fld_class();
    
    keyb_class  * keyb  = new keyb_class(p_PWM_ADJUST,p_TRAVEL_ADJUST,p_forward,p_backward,p_stop, p_learn);    
    plan_class();
    void onKey_change(uint16_t key_pin, uint16_t new_val, bool is_adc);
    //bool ready_to_work = false;
    uint8_t max_feed_rate = 100; // mm/s 
    
    volatile uint16_t calc1 = 0,calc2 = 0,calc_0 = 0, tim2 = 0;
    void inter();
    void plan_inter();
    volatile uint8_t run_motor_to_pos(             uint8_t motor_num,int32_t dest_pos, uint8_t max_pwm_speed, bool init_mode = false,uint8_t cmd_num=0,uint8_t cmd_index=0,bool try_take_access=false,bool in_free_cmd_on_take_dest = false);
    // void    run_motor_to_pos_in_percents(uint8_t motor_num,uint8_t percents, uint8_t max_pwm_speed, uint8_t cmd_num=0,uint8_t cmd_index=0);
    // void    external_stop_resume_motor(    uint16_t motor_num,bool external_stopped);
    // void    set_motor_global_motor_max_pwm_power(uint16_t motor_num,uint8_t new_global_motor_max_pwm_power);
    volatile void    init_motors_max_pos();
    volatile void    go_all_home_pos();
    volatile void    break_last_command_for_motor(uint16_t motor_num);
    void    p_backward_pressed();
    void    p_forward_pressed();

    volatile bool init_already_run = false;

    // bool read_travel_pin = false, read_max_speed_pin = false;
    bool read_travel_pin = true, read_max_speed_pin = true;
    // ----------------------commands here!
    uint32_t gcode_N, gcode_LastN, Stopped_gcode_LastN,previous_millis_cmd = 0;
    char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];
    char serial_char = ' ';
    bool comment_mode = false;
    uint8_t serial_count = 0,bufindr = 0,buflen = 0,bufindw = 0, ALL_MOTORS_INDEX=10;
    char *strchr_pointer; 

    uint32_t code_value();
    bool     code_seen(char code);

    void get_command();
    void process_commands();

    void store_to_eeprom();
    void load_from_eeprom();

    void FlushSerialRequestResend();
    void ClearToSend();
    void cmd_loop();
    // void cmd_loop2();
    void enquecommand(const char *cmd); //put an ASCII command at the end of the current buffer.
    void enquecommand_P(const char *cmd); //put an ASCII command at the end of the current buffer, read from flash
    void send_debug_info();
    void free_all_motors_from_cmd(uint8_t cmd_num,uint8_t cmd_index[4], bool ignore_cmd_access);

      // motor      1   2   3   4
      // Current    A12 A13 A14 A15
      // Direction  42  43  44  45
      // PWM        8   7   6   5
      // Hall A     34  36  38  40
      // Hall B     35  37  39  41
          


            //------------------------motors here

    // const int pwm_a = 8;  //PWM for CH1 LF
    // const int pwm_b = 9;  //PWM for CH4 RR  
    // const int pwm_c = 10;  //PWM for CH2 LR
    // const int pwm_d = 11;  //PWM for CH3 RF

    const int pwm_a = 8;  //PWM for CH1 LF
    const int pwm_b = 7;  //PWM for CH4 RR  
    const int pwm_c = 6;  //PWM for CH2 LR
    const int pwm_d = 5;  //PWM for CH3 RF
    // const int dir_a = 7;  //DIR for CH1
    const int dir_a = 42;  //DIR for CH1
    const int dir_b = 43;  //DIR for CH4
    const int dir_c = 44;  //DIR for CH2
    const int dir_d = 45;  //DIR for CH3

    //Current Sensors
    const int CURRENTA = A12; 
    const int CURRENTB = A13; 
    const int CURRENTC = A14; 
    const int CURRENTD = A15; 
    const int CURRENT_LIMIT = (1024 / 5) * 2.6;  // amps


    Dagu4Motor *motor[4] = { new Dagu4Motor(pwm_a, dir_a, CURRENTA,34,35),//
                            new Dagu4Motor(pwm_b, dir_b, CURRENTB,36,37),
                            new Dagu4Motor(pwm_c, dir_c, CURRENTC,38,39),
                            new Dagu4Motor(pwm_d, dir_d, CURRENTD,40,41)};

    
    
// current sensing please use this, per motor, this I believe puts out 1 volt per amp , please set for 3 Amps. if exceed the max current of 3A the controller will stop. 
 
// control... 
// you may select the pins on the mega. 
 
// a)  pin ( you decide) is high , pin goes low all motors move forward, if goes high motors stop. 
 
// b)  pin ( you decide) is high , pin goes low all motors move backward, if goes high motors stop. 
 
// c)  pin ( you decide) is high , pin goes low all motors stop, 
 
// d)   pin ( you decide) is high , pin goes low, resets counter, then slowly moves motors all the way out and counts the pulses ( when no pulses counted store count in eprom) then rerun motors home,( this is where & nbsp; d) below is used to adjust travel. 
 
// d)  pin ( you decide) , pin 0-5v this will adjust travel limit 
 
// e) when motors are moving only the stop button can stop the operation. 


};






#endif

