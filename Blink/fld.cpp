
#include "fld.h"
#include <avr/interrupt.h>
#include <avr/io.h>



void fld_class::inter(){
  set_time(500,1);
  calc_1++;  
}

fld_class::fld_class(){
   
}


