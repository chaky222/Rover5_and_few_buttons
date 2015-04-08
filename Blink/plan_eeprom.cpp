
#include "plan.h"
#include <avr/eeprom.h>

void _EEPROM_writeData(int &pos, uint8_t* value, uint8_t size)
{
    do
    {
        eeprom_write_byte((unsigned char*)pos, *value);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_WRITE_VAR(pos, value) _EEPROM_writeData(pos, (uint8_t*)&value, sizeof(value))
void _EEPROM_readData(int &pos, uint8_t* value, uint8_t size)
{
    do
    {
        *value = eeprom_read_byte((unsigned char*)pos);
        pos++;
        value++;
    }while(--size);
}
#define EEPROM_READ_VAR(pos, value) _EEPROM_readData(pos, (uint8_t*)&value, sizeof(value))
#define EEPROM_VERSION "V10"
#define EEPROM_OFFSET 100

void plan_class::store_to_eeprom(){
  // char ver[4]= "000";
  int i=EEPROM_OFFSET;
  // EEPROM_WRITE_VAR(i,ver);
  for (uint8_t u = 0;u<4;u++){ 
    EEPROM_WRITE_VAR(i,motor[u]->max_position);
  }
  for (uint8_t u = 0;u<4;u++){ 
    uint8_t v_tmp = (motor[u]->invert_direction) ? 200 : 0;
    EEPROM_WRITE_VAR(i,v_tmp);
  }
  // char ver2[4]=EEPROM_VERSION;
  // i=EEPROM_OFFSET;
  // EEPROM_WRITE_VAR(i,ver2); // validate data
}
void plan_class::load_from_eeprom(){
    int i=EEPROM_OFFSET;
    // char stored_ver[4];
    // char ver[4]=EEPROM_VERSION;
    // EEPROM_READ_VAR(i,stored_ver); //read stored version
    //  SERIAL_ECHOLN("Version: [" << ver << "] Stored version: [" << stored_ver << "]");
    // if (strncmp(ver,stored_ver,3) == 0){
      for (uint8_t u = 0;u<4;u++){ 
        EEPROM_READ_VAR(i,motor[u]->max_position);             
      }
      for (uint8_t u = 0;u<4;u++){ 
        uint8_t v_tmp = 0;
        EEPROM_READ_VAR(i,v_tmp);
        bool new_val = (v_tmp>0) ? true : false;
        motor[u]->motor_dir = (motor[u]->invert_direction == new_val) ? motor[u]->motor_dir : !motor[u]->motor_dir;
        motor[u]->invert_direction = new_val;
        motor[u]->init_done = true;  
      }
      // SERIAL_ECHOPGM("[CMD] [HIDDEN] G16"); for (uint8_t u = 0;u<4;u++){ SERIAL_ECHOPAIR(" ", motor[u].max_position); } 
      // ClearToSend();      
    // }
}