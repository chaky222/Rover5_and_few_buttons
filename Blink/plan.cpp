
#include "plan.h"
#include <avr/interrupt.h>
#include <avr/io.h>

void plan_class::inter(){
  if (cnt->current_item<255){
    uint8_t curr = cnt->current_item;
    cnt->current_item = cnt->ints[cnt->current_item].next_item;
    if (cnt->current_item<255){
      // cnt->ints[cnt->current_item].tmp = curr;
      TCNT1 = cnt->ints[cnt->current_item].interval;       
    }
    // и всёже выполним процедуру по таймеру
    cnt->ints[curr].functionPtr();
  }
}

void plan_class::plan_inter(){
  send_debug_info();
  // calc2++;
}

void plan_class::send_debug_info(){
  SERIAL_ECHOPGM("[CMD] [HIDDEN] G2"); for (uint8_t u = 0;u<4;u++){ SERIAL_ECHOPAIR(" ", motor[u]->getCurrent()); }        
  SERIAL_ECHOPGM(" \n [CMD] [HIDDEN] G3");  for (uint8_t u = 0;u<4;u++){ SERIAL_ECHOPAIR(" ", motor[u]->position);}
  SERIAL_ECHOPGM(" \n [CMD] [HIDDEN] G16");  for (uint8_t u = 0;u<4;u++){ SERIAL_ECHOPAIR(" ", motor[u]->max_position);}
  SERIAL_ECHOPAIR(" \n [CMD] [HIDDEN] G10 ", keyb->keys[1]->old_value);
  
  for (uint16_t u = 0;u<4;u++){
    SERIAL_ECHOPAIR(" \n [CMD] [HIDDEN] G15 ", u);
    SERIAL_ECHOPAIR(" ", motor[u]->dest_position);
  }
  for (int8_t u = 0;u<4;u++){ 
    if (motor[u]->error_code>0){
      SERIAL_ECHOPAIR(" \n ERROR CODE ",motor[u]->error_code);
      SERIAL_ECHOPAIR(" in motor  ",u);
      motor[u]->error_code = 0;
    }          
  }
 
  
  ClearToSend();  

  // SERIAL_ECHOPAIR(" \n b=",         motor[0].motor_is_blocked_now); 
  // SERIAL_ECHOPAIR(" k0=",           keyb->keys[0].value);  
  // SERIAL_ECHOPAIR(" k1=",           keyb->keys[1].value);  
  // SERIAL_ECHOPAIR(" k2=",           keyb->keys[2].value);  
  // SERIAL_ECHOPAIR(" k3=",           keyb->keys[3].value);       
  // SERIAL_ECHOPAIR(" dest=",         motor[0].dest_position);   
  // SERIAL_ECHOPAIR(" debug_tmp=",         motor[0].debug_tmp);  
  // SERIAL_ECHOPAIR(" max_position=",         motor[0].max_position);                       
  // SERIAL_ECHOPAIR(" \n zero_state=",      motor[0].zero_state);       motor[0].zero_state=0; 
  // SERIAL_ECHOPAIR("  non_zero_state=",  motor[0].non_zero_state);   motor[0].non_zero_state=0; 
  // SERIAL_ECHOPAIR("  mc=", micros() );         
  // SERIAL_ECHOPAIR("  calc2=", calc2);   calc2=0;
  // SERIAL_ECHOPAIR("  calc1=", calc1);       calc1=0; 
  // SERIAL_ECHOPAIR("  fld=", fld->calc_1);     fld->calc_1=0;
  //   SERIAL_ECHOPGM(" \n time_left = "); 
  SERIAL_ECHOPAIR("\n  calc_keyb=", keyb->calc_keyb);       keyb->calc_keyb=0; 
  // for (int8_t u = 0;u<4;u++){ 
  //   SERIAL_ECHOPAIR(" m", motor[u]->global_motor_max_pwm_power); 
  // }
  // SERIAL_ECHOPAIR("  mc=", micros() );  
  uint16_t c2;
  c2 = tim2; tim2=0;        
  SERIAL_ECHOPAIR("  tim2=", c2);   
  uint16_t c1;
  c1 = calc1; calc1=0; 
  SERIAL_ECHOPAIR("  calc1=", c1);    
  for (int8_t u = 0;u<4;u++){ 
    SERIAL_ECHOPAIR(" tm", motor[u]->invert_direction); 
  } 

  // int16_t k2 = keyb->keys[2]->old_value; SERIAL_ECHOPAIR("  k2=", k2); 
  // int16_t k3 = keyb->keys[3]->old_value; SERIAL_ECHOPAIR("  k3=", k3); 
  // int16_t k4 = keyb->keys[4]->old_value; SERIAL_ECHOPAIR("  k4=", k4); 
  // int16_t k5 = keyb->keys[5]->old_value; SERIAL_ECHOPAIR("  k5=", k5); 
        
  // SERIAL_ECHOPAIR("  fld=", fld->calc_1);     fld->calc_1=0;
  ClearToSend();

}

plan_class::plan_class(){

}


void plan_class::onKey_change(uint16_t key_pin, uint16_t new_val, bool is_adc){
  // SERIAL_ECHOPAIR(" \n key pressed k=",         key_pin); 
  // SERIAL_ECHOPAIR(" v=",                        new_val); 
  // SERIAL_ECHOPAIR(" adc=",                      is_adc); 
  // ClearToSend();
  // return ;
  switch(key_pin){
      case p_PWM_ADJUST: // p_ADC1 = pwm ,
        if (read_max_speed_pin){
          // SERIAL_ECHOPAIR("[CMD] [HIDDEN] G11 ", new_val); ClearToSend();
          // for (uint8_t u = 0;u<4;u++){ 
          //   // motor[u]->global_motor_max_pwm_power = new_val; 
          //   // set_motor_global_motor_max_pwm_power(u,new_val); 
          // }   

        }
        break;
      case p_TRAVEL_ADJUST: // p_ADC2 = distance in persents
        if (read_travel_pin){
          // SERIAL_ECHOPAIR("[CMD] [HIDDEN] G10 ", new_val); ClearToSend();
          // uint8_t persents = (new_val*100/255);          
          // for (uint8_t u = 0;u<4;u++){ 
          //   // run_motor_to_pos_in_percents(u, new_val, motor[u]->global_motor_max_pwm_power);
          // }        
        }
        break;
      case p_forward: 
        if (new_val==VK_KEY_DOWN){
          enquecommand("G6");
        }   
        break;
      case p_backward: 
        // uint8_t adc_val = analogRead(keyb->keys[2].pin )/4;
        // int32_t new_dest = 0;
        if (new_val==VK_KEY_DOWN){
          enquecommand("G5");
          // p_backward_pressed();
        }
        
        break;
      case p_stop: 
        if (new_val==VK_KEY_DOWN){
          break_last_command_for_motor(ALL_MOTORS_INDEX);
        }
        // for (uint8_t u = 0;u<4;u++){ 

        //   // external_stop_resume_motor(u ,(new_val>0));
        // }
        
        break;
      case p_learn: 
        if (new_val==VK_KEY_DOWN){  
          if (!init_already_run){ 
            enquecommand("G9");  
          }
        }
        break;
      
      default: 
        break;
  }  
}

volatile void plan_class::break_last_command_for_motor(uint16_t motor_num){
  // SERIAL_ECHOPAIR("\n  break_last_command_for_motor=", motor_num);      
  for (uint8_t u = 0;u<4;u++){ 
    if ((motor_num == u)||(motor_num == ALL_MOTORS_INDEX)){
      motor[u]->free_motor_from_cmd(0,0,true);
      motor[u]->set_dest_position(motor[u]->position,0,0,true,false,false);
    }    
  }
}

void plan_class::p_backward_pressed(){  
  int16_t adc_val = keyb->keys[1]->old_value;    
  for (uint8_t u = 0;u<4;u++){
    // int32_t new_dest = motor[u]->position - ((motor[u]->max_position*adc_val)/255);  
    int32_t new_dest = 0;
    uint16_t new_acc = motor[u]->get_access(31);  
    // bool new_acc2 = motor[u]->check_cmd_access(31,new_acc, false);        
    // SERIAL_ECHOPAIR("\n  p_backward_pressed new_acc=", new_acc); 
    // SERIAL_ECHOPAIR(" new_acc2=", new_acc2);    
    // SERIAL_ECHOPAIR(" check1=", motor[u]->check_cmd_access(31,new_acc));    
    // SERIAL_ECHOPAIR(" check2=", motor[u]->check_cmd_access(31,new_acc2));    

    run_motor_to_pos(u,new_dest,motor[u]->global_motor_max_pwm_power,false,31,new_acc,false, true);
  }
}

void plan_class::p_forward_pressed(){
  int16_t adc_val = keyb->keys[1]->old_value;        
  for (uint8_t u = 0;u<4;u++){ 
    int32_t new_dest = motor[u]->position + ((motor[u]->max_position*adc_val)/255);
    uint16_t new_acc = motor[u]->get_access(32);     
    // bool new_acc2 = motor[u]->check_cmd_access(32,new_acc,false);     
    // SERIAL_ECHOPAIR("\n  p_forward_pressed acc=", new_acc);   
    // SERIAL_ECHOPAIR(" new_acc2=", new_acc2);  
    // uint8_t plan_class::run_motor_to_pos(uint8_t motor_num,int32_t dest_pos, uint8_t max_pwm_speed, bool init_mode, uint8_t cmd_num,uint8_t cmd_index, bool try_take_access,bool in_free_cmd_on_take_dest){
    run_motor_to_pos(u, new_dest, motor[u]->global_motor_max_pwm_power, false,32,new_acc,false,true);
  }  
}

//-------------------------------------------------------------command block

//adds an command to the main command buffer
//thats really done in a non-safe way.
//needs overworking someday
void plan_class::enquecommand(const char *cmd)
{
  if(buflen < BUFSIZE)
  {
    //this is dangerous if a mixing of serial and this happens
    strcpy(&(cmdbuffer[bufindw][0]),cmd);
    // SERIAL_ECHO_START;
    // SERIAL_ECHOPGM("enqueing \"");
    // SERIAL_ECHO(cmdbuffer[bufindw]);
    // SERIAL_ECHOLNPGM("\"");
    bufindw= (bufindw + 1)%BUFSIZE;
    buflen += 1;
  }
}

void plan_class::enquecommand_P(const char *cmd)
{
  if(buflen < BUFSIZE)
  {
    //this is dangerous if a mixing of serial and this happens
    strcpy_P(&(cmdbuffer[bufindw][0]),cmd);
    SERIAL_ECHO_START;
    SERIAL_ECHOPGM("enqueing \"");
    SERIAL_ECHO(cmdbuffer[bufindw]);
    SERIAL_ECHOLNPGM("\"");
    bufindw= (bufindw + 1)%BUFSIZE;
    buflen += 1;
  }
}



// void plan_class::cmd_loop2(){
//    while(1){
//     tim2++;  
//     delay(10);
//   }
// }
void plan_class::cmd_loop(){

    if(buflen < (BUFSIZE-1)) get_command();  
    if(buflen)
    {
      process_commands();   
      buflen = (buflen-1);
      bufindr = (bufindr + 1)%BUFSIZE;
    }
    
  // check_keyboard();
  // p->check_encoders();
  // _delay_ms(1000);

}
void plan_class::ClearToSend() {
  previous_millis_cmd = millis();
  SERIAL_PROTOCOLLNPGM(MSG_OK);
}

void plan_class::FlushSerialRequestResend() {
  //char cmdbuffer[bufindr][100]="Resend:";
  Serial.flush();
  SERIAL_PROTOCOLPGM(MSG_RESEND);
  SERIAL_PROTOCOLLN(gcode_LastN + 1);
  ClearToSend();
}

void plan_class::get_command() {
  while( Serial.available() > 0  && buflen < BUFSIZE) {
    serial_char = Serial.read();
    // yield();
    if(serial_char == '\n' ||
       serial_char == '\r' ||
       (serial_char == ':' && comment_mode == false) ||
       serial_count >= (MAX_CMD_SIZE - 1) )
    {
      if(!serial_count) { //if empty line
        comment_mode = false; //for new command
        return;
      }
      cmdbuffer[bufindw][serial_count] = 0; //terminate string
      if(!comment_mode){
        comment_mode = false; //for new command

        if(strchr(cmdbuffer[bufindw], 'N') != NULL)
        {
          strchr_pointer = strchr(cmdbuffer[bufindw], 'N');
          gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
          
          if(gcode_N != gcode_LastN+1 && (strstr_P(cmdbuffer[bufindw], PSTR("M110")) == NULL) ) {
            SERIAL_ERROR_START;
            SERIAL_ERRORPGM(MSG_ERR_LINE_NO);
            SERIAL_ERRORLN(gcode_LastN);
            //Serial.println(gcode_N);
            FlushSerialRequestResend();
            serial_count = 0;
            return;
          }

          if(strchr(cmdbuffer[bufindw], '*') != NULL)
          {
            byte checksum = 0;
            byte count = 0;
            while(cmdbuffer[bufindw][count] != '*') checksum = checksum^cmdbuffer[bufindw][count++];
            strchr_pointer = strchr(cmdbuffer[bufindw], '*');

            if( (int)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum) {
              SERIAL_ERROR_START;
              SERIAL_ERRORPGM(MSG_ERR_CHECKSUM_MISMATCH);
              SERIAL_ERRORLN(gcode_LastN);
              FlushSerialRequestResend();
              serial_count = 0;
              return;
            }
            //if no errors, continue parsing
          }
          else
          {
            SERIAL_ERROR_START;
            SERIAL_ERRORPGM(MSG_ERR_NO_CHECKSUM);
            SERIAL_ERRORLN(gcode_LastN);
            FlushSerialRequestResend();
            serial_count = 0;
            return;
          }

          gcode_LastN = gcode_N;
          //if no errors, continue parsing
        }
        else  // if we don't receive 'N' but still see '*'
        {
          if((strchr(cmdbuffer[bufindw], '*') != NULL))
          {
            SERIAL_ERROR_START;
            SERIAL_ERRORPGM(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
            SERIAL_ERRORLN(gcode_LastN);
            serial_count = 0;
            return;
          }
        }        

        //If command was e-stop process now
        // if(strcmp(cmdbuffer[bufindw], "G7") == 0)  break_last_command_for_motor(ALL_MOTORS_INDEX);
        
        
        bufindw = (bufindw + 1)%BUFSIZE;
        buflen += 1;
      }
      serial_count = 0; //clear buffer
    }
    else
    {
      if(serial_char == ';') comment_mode = true;
      if(!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
    }
  }
}

uint32_t plan_class::code_value()
{
  return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

bool plan_class::code_seen(char code)
{
  strchr_pointer = strchr(cmdbuffer[bufindr], code);
  return (strchr_pointer != NULL);  //Return True if a character was found
}

volatile uint8_t plan_class::run_motor_to_pos(uint8_t motor_num,int32_t dest_pos, uint8_t max_pwm_speed, bool init_mode, uint8_t cmd_num,uint8_t cmd_index, bool try_take_access,bool in_free_cmd_on_take_dest){
  if (!init_mode){
    if ( (!motor[motor_num]->init_done)) return 0;
    dest_pos = (dest_pos<0)? 0 : (dest_pos > motor[motor_num]->max_position) ? motor[motor_num]->max_position : dest_pos;
  }
  motor[motor_num]->set_dest_position(dest_pos, cmd_num, cmd_index,false, in_free_cmd_on_take_dest);
  // motor[motor_num]->set_dest_position(dest_pos, cmd_num, cmd_index,try_take_access, in_free_cmd_on_take_dest);
  // uint16_t tmp = motor_num, tmp2=cmd_num, tmp3=cmd_index ; 
  // SERIAL_ECHOPAIR(" \n motor_num=", tmp);
  // SERIAL_ECHOPAIR(" cmd_num=", tmp2);
  // SERIAL_ECHOPAIR(" cmd_index=", tmp3);
  // SERIAL_ECHOPAIR(" try_take_access=", try_take_access);
  // SERIAL_ECHOPAIR(" in_free_cmd_on_take_dest=", in_free_cmd_on_take_dest);
  // ClearToSend();
  motor[motor_num]->setSpeed(0,cmd_num,cmd_index, false,false,false);
  return motor[motor_num]->setSpeed(max_pwm_speed,cmd_num,cmd_index,try_take_access,false,in_free_cmd_on_take_dest);
}

// void plan_class::run_motor_to_pos_in_percents(uint8_t motor_num,uint8_t percents, uint8_t max_pwm_speed, uint8_t cmd_num,uint8_t cmd_index){
//   int32_t new_dest = motor[motor_num]->max_position*percents/100;
//   run_motor_to_pos(motor_num,new_dest,motor[motor_num]->global_motor_max_pwm_power,false,cmd_num,cmd_index , false);
// }


// void plan_class::set_motor_global_motor_max_pwm_power(uint16_t motor_num,uint8_t new_global_motor_max_pwm_power){
//   motor[motor_num]->global_motor_max_pwm_power = new_global_motor_max_pwm_power; 
//   if ((motor[motor_num]->is_run_now) && (motor[motor_num]->init_done)){ motor[motor_num]->setSpeed(new_global_motor_max_pwm_power,motor[motor_num]->cmd_num_current, motor[motor_num]->cmd_index_current,false );}
// }

volatile void plan_class::go_all_home_pos(){
  int8_t stay_offset = 5;
  uint8_t home_axe_cmd_num = 25;
  uint8_t current_cmd_index[4] = {0,0,0,0};
  for (uint8_t u = 0;u<4;u++){   
    motor[u]->e->write(0); 
    motor[u]->init_done = false;
    current_cmd_index[u] = motor[u]->get_access(home_axe_cmd_num);
    run_motor_to_pos(u,-100000,motor[u]->global_motor_max_pwm_power/4,true,home_axe_cmd_num,current_cmd_index[u], false);     
  }
  bool is_init = true;
  while (is_init){
    is_init=false;
    _delay_ms(100);
    for (uint8_t u = 0;u<4;u++){  
      if (!motor[u]->check_cmd_access(home_axe_cmd_num,current_cmd_index[u], false)) return;
      if (!motor[u]->motor_is_blocked_now){
        is_init = true;              
      }
    }          
  }
  for (uint8_t u = 0;u<4;u++){ 
    if (!motor[u]->check_cmd_access(home_axe_cmd_num,current_cmd_index[u], false)) return;  
    motor[u]->e->write(0-stay_offset); 
    motor[u]->set_dest_position(motor[u]->position,home_axe_cmd_num, current_cmd_index[u],false);
    motor[u]->init_done = true;
    if (motor[u]->max_position > 0){
      run_motor_to_pos(u,0,motor[u]->global_motor_max_pwm_power, false,home_axe_cmd_num,current_cmd_index[u], false);     
    } 
    motor[u]->free_motor_from_cmd(home_axe_cmd_num,current_cmd_index[u]);
  }
  // SERIAL_ECHOPAIR(" \n HOME all done ",1234);
  // ClearToSend();
}

void plan_class::free_all_motors_from_cmd(uint8_t cmd_num,uint8_t cmd_index[4], bool ignore_cmd_access){
  for (uint8_t u = 0;u<4;u++){
    motor[u]->set_dest_position(motor[u]->position, cmd_num, cmd_index[u], false);
    motor[u]->free_motor_from_cmd(cmd_num, cmd_index[u]);    
  }
}

volatile void plan_class::init_motors_max_pos(){
  init_already_run = true;
  uint8_t init_motor_cmd_num = 29;
  uint8_t current_cmd_index[4] = {0,0,0,0};
  int16_t first_motor_pos[4] = {0,0,0,0};
  for (uint8_t u = 0;u<4;u++){      
    current_cmd_index[u] = motor[u]->get_access(init_motor_cmd_num);
    if (!motor[u]->check_cmd_access(init_motor_cmd_num,current_cmd_index[u], false)) {
      free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);  return ; 
    }
  }
  int8_t stay_offset = 5;  
  for (uint8_t u = 0;u<4;u++){   
    // motor[u]->e->write(0); 
    // motor[u]->init_done = false;
    // motor[u]->external_stopped  =false;
    first_motor_pos[u] = motor[u]->position;
    run_motor_to_pos(u,-100000,motor[u]->global_motor_max_pwm_power/4,true,init_motor_cmd_num,current_cmd_index[u], false);         
  }
  bool is_init;
  is_init = true;
  while (is_init){
    is_init=false;
    _delay_ms(100);    
    for (uint8_t u = 0;u<4;u++){  
      if (!(motor[u]->check_cmd_access(init_motor_cmd_num,current_cmd_index[u], false))) {
        free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);  return ; 
      }
      if (!motor[u]->motor_is_blocked_now){
        is_init = true;              
      }
    } 
    // return ;         
  }
  for (uint8_t u = 0;u<4;u++){       
    if (!motor[u]->check_cmd_access(init_motor_cmd_num,current_cmd_index[u], false)) {
      free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);  return ; 
    }
    if (motor[u]->position > first_motor_pos[u]){ // this is mean that motor mooves in other direction!
      motor[u]->invert_direction = !motor[u]->invert_direction;
      motor[u]->motor_dir        = !motor[u]->motor_dir;
      
      int16_t ssss = u;
      SERIAL_ECHOPAIR(" \n\n zzzz=", motor[u]->invert_direction); 

      SERIAL_ECHOPAIR(" \n\n ", 0);  
      free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);
      init_motors_max_pos();
      return ;
    }
    motor[u]->e->write(0-stay_offset); 
    first_motor_pos[u] = 0-stay_offset;
    run_motor_to_pos(u,100000,motor[u]->global_motor_max_pwm_power/4,true,init_motor_cmd_num,current_cmd_index[u], false);  
  }
  is_init = true;
  while (is_init){
    is_init=false;
    _delay_ms(100);    
    for (uint8_t u = 0;u<4;u++){  
      if (!motor[u]->check_cmd_access(init_motor_cmd_num,current_cmd_index[u], false)){
        free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);  return ; 
      } 
      if (!motor[u]->motor_is_blocked_now){
        is_init = true;              
      }
    }          
  }
  for (int16_t u = 0;u<4;u++){   
    if (!motor[u]->check_cmd_access(init_motor_cmd_num,current_cmd_index[u], false)){
      free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);  return ; 
    } 
    if (motor[u]->position < first_motor_pos[u]){ // this is mean that motor mooves in other direction!
      motor[u]->invert_direction = !motor[u]->invert_direction;
      motor[u]->motor_dir        = !motor[u]->motor_dir;
      free_all_motors_from_cmd(init_motor_cmd_num, current_cmd_index, false);
      init_motors_max_pos();
      return ;
    }
    // SERIAL_ECHOPAIR(" \n motor_num=", u);
    // SERIAL_ECHOPAIR(" b=", b);
    // int16_t tmp = motor[u]->cmd_num_current;       SERIAL_ECHOPAIR(" mc=", tmp);
    // int16_t tmp2 = init_motor_cmd_num;            SERIAL_ECHOPAIR(" c=",  tmp2);
    // int16_t tmp3 = motor[u]->cmd_index_current;    SERIAL_ECHOPAIR(" mi=", tmp3);
    // int16_t tmp4 = current_cmd_index[u];          SERIAL_ECHOPAIR(" i=",  tmp4);
    
    motor[u]->max_position = (motor[u]->position > stay_offset) ? (motor[u]->position-stay_offset) : 0;
    motor[u]->set_dest_position(motor[u]->position, init_motor_cmd_num, current_cmd_index[u],false);
    motor[u]->init_done = true;
    if (motor[u]->position > 0){
      run_motor_to_pos(u,0,motor[u]->global_motor_max_pwm_power, false,init_motor_cmd_num,current_cmd_index[u], false);     
    } 
    motor[u]->free_motor_from_cmd(init_motor_cmd_num, current_cmd_index[u], false);
  }
  store_to_eeprom();
  // SERIAL_ECHOPGM("[CMD] [HIDDEN] G16"); for (uint8_t u = 0;u<4;u++){ SERIAL_ECHOPAIR(" ", motor[u]->max_position); } 
  // ClearToSend();
}

void plan_class::process_commands() {
  unsigned long codenum; //throw away variable
  char *starpos = NULL;
  if(code_seen('G')) {
    switch((int)code_value()){
      case 0: // G0 -> G1
      case 1: // G1
        SERIAL_ECHOLNPGM("------------G1 RUN-----------------");   
        // if(Stopped == false) {
        //   //ClearToSend();
        //   return;
        // }
        break;
      case 2: // G2 Get currencies and pos from all motors 
      // send_debug_info();
        
      
        break;
      case 3: // G3 G3 and G4 for debag encoders.
        // motor[0].setMotorDirection(motor[0].motor_dir<1); 
        // motor[0].setMotorDirection(true); 
        // motor[0].setSpeed(250);
        // digitalWrite(50, LOW);
        // digitalWrite(52, LOW);        
        break;
      case 4: // G4 
        digitalWrite(50, HIGH);
        digitalWrite(52, HIGH);        
        break;
      case 5: // G5 Run one motor NOW!
        p_backward_pressed();
        
        // if(code_seen('M')){
        //   uint16_t motor_num = code_value();
        //   // SERIAL_ECHOPAIR(" motor_num ", motor_num);
        //   bool dir = false;
        //   if ((code_seen('D')) && (code_value()>0)) dir = true;
        //   uint16_t motor_speed = 100;
        //   if (code_seen('P')) motor_speed =code_value();
        //   // SERIAL_ECHOPAIR(" motor_speed ", motor_speed);
        //   SERIAL_ECHOPAIR("[CMD] [HIDDEN] G5 ", motor_num);
        //   SERIAL_ECHOPAIR(" ", dir);
        //   ClearToSend();  
        //   // SERIAL_ECHOPGM(" \n");   
        //   motor[motor_num]->setMotorDirection(dir); 
        //   motor[motor_num]->setSpeed(motor_speed);
        //   // p->boolMove = true;
        // }    
        break;
      case 6: // G6 resume after external stop motors NOW!
         p_forward_pressed();
      
        // if(code_seen('M')){ 
        //   uint16_t motor_num = code_value();         
        //   external_stop_resume_motor(motor_num, false); 
        // }else{
        //   external_stop_resume_motor(ALL_MOTORS_INDEX, false); 
        // } 
        // if(code_seen('D')){          
        //   bool dir = false;
        //   if ((code_seen('D')) && (code_value()>0)) dir = true;
        //   uint16_t motor_speed = 100;
        //   if (code_seen('P')) motor_speed =code_value();
        //   SERIAL_ECHOPAIR(" run all motor_speed ", motor_speed);
        //   for (uint8_t u = 0;u<4;u++){
        //     // p->motor[u]->setMotorDirection(dir); 
        //     // p->motor[u]->setSpeed(motor_speed);
        //   }
        //   // p->boolMove = true;
        // }    
        break;
      case 7: // G7 external stop motors NOW!
        break_last_command_for_motor(ALL_MOTORS_INDEX);
        // if(code_seen('M')){          
        //   uint16_t motor_num = code_value();
        //   external_stop_resume_motor(motor_num, true);      
        // }else{
        //   external_stop_resume_motor(ALL_MOTORS_INDEX, true);
        // }    
        break;
      case 8: // G8 write encoder position now to zero
        for (uint8_t u = 0;u<4;u++){          
          motor[u]->e->write(0); 
        }

        // mStop();  
        // SERIAL_ECHOPGM("[CMD] [HIDDEN] G8 "); 
        // SERIAL_ECHOPGM(" stop all motors \n");   
        break;
      case 9: // G9 init now.  
        // SERIAL_ECHOPGM(" INIT RUN \n ");  
        init_motors_max_pos();    
        init_already_run = false;     
        break;
      case 10: // G10 run to dest in percent
        if (!read_travel_pin){
          bool mots[4]; mots[0] = 0;mots[1] = 0;mots[2] = 0;mots[3] = 0;
          if(code_seen('M')){          
            uint16_t motor_num = code_value();
            mots[motor_num] = 1;   
          }else{
            mots[0] = 1;mots[1] = 1;mots[2] = 1;mots[3] = 1;
          }  
          uint8_t percents; percents = 50; if (code_seen('P')) percents =code_value();          
          for (uint8_t u = 0;u<4;u++){ 
            // if (mots[u]){ run_motor_to_pos_in_percents(u,percents,motor[u]->global_motor_max_pwm_power,0,0); }
          }
        }
        break;
      case 11: // G11 set global_motor_max_pwm_power
        if (!read_max_speed_pin){
          uint16_t motor_speed = 100; if (code_seen('P')) motor_speed =code_value();  motor_speed = constrain(motor_speed, 0, 255);
          if(code_seen('M')){          
            uint16_t motor_num = code_value();   
            // set_motor_global_motor_max_pwm_power(motor_num,motor_speed);      
           
          }else{
            for (uint8_t u = 0;u<4;u++){ 
              // set_motor_global_motor_max_pwm_power(u,motor_speed); 
            }
          } 
        }

        break;
      case 12: // G12 read_travel_pin[T], read_max_speed_pin[S]         
          read_travel_pin =     (code_seen('T')) ? (code_value() > 0 ) : 1;
          read_max_speed_pin =  (code_seen('S')) ? (code_value() > 0 ) : 1;        
        break;
      // case 14: //G14 set new power[P] and distance[D];        
      //   // if (!read_travel_pin){
      //   //   int32_t new_dest = (code_seen('P')) ? (code_value() ) : 50;
      //   //   for (uint8_t u = 0;u<4;u++){ 
      //   //     run_motor_to_pos_in_percents(u,new_dest,motor[u]->global_motor_max_pwm_power); 
      //   //   }
      //   // }
      //   // if (!read_max_speed_pin){
      //   //   uint16_t motor_speed = (code_seen('D')) ? (code_value() ) : 100;
      //   //   for (uint8_t u = 0;u<4;u++){ 
      //   //     set_motor_global_motor_max_pwm_power(u,motor_speed); 
      //   //   }
      //   // }
      //   break;
      default:
        SERIAL_ECHO_START;
        SERIAL_ECHOPGM(MSG_UNKNOWN_COMMAND);
        SERIAL_ECHO(cmdbuffer[bufindr]);
        SERIAL_ECHOLNPGM("\"");
        break;      
    }
  }else {
    SERIAL_ECHO_START;
    SERIAL_ECHOPGM(MSG_UNKNOWN_COMMAND);
    SERIAL_ECHO(cmdbuffer[bufindr]);
    SERIAL_ECHOLNPGM("\"");
  }

  // ClearToSend();
}





