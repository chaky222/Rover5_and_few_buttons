
#include "cnt.h"
#include <avr/interrupt.h>
#include <avr/io.h>



void cnt_class::set_time(uint16_t tacts, uint8_t index){
    // тут нам надо проанализировать кто за кем идёт и воткнуться в нужное место.
    // а также пересчитать всех кто идёт после.... 
    interrupts_stek_item * in_item = &ints[index];
    in_item->interval = ~tacts;
    interrupts_stek_item * c_item;
    // in_item->tmp =5 ;

    // итак 1-е: находим место куда мы должны воткнуться. 
    // c_index  - после кого мы должны воткнуться
    // n_index  - кто после нас будет
    // теперь задача сводиться только к поиску c_index-а


    uint8_t c_index = 255;
    // uint16_t c_tacts3 = 0;
    
    if (current_item<255){
      c_item = &ints[current_item];
      // ints[current_item]->tmp = index;
      // ints[index]->tmp = current_item;
      c_item->interval = TCNT1;
      if (c_item->interval >= in_item->interval ){
        // ints[current_item].tmp = 7;
        c_index = current_item; 
        tacts = ~c_item->interval;
        in_item->interval = in_item->interval  + tacts;

        uint8_t  nxt_index = c_item->next_item;
        while ((nxt_index<255) && (ints[nxt_index].interval >= in_item->interval )) {// надо выбрать между какими из следующихх нам бы приткнуться->->-> это самый длинный процес сдесь            
          tacts = ~ints[nxt_index].interval;
          in_item->interval = in_item->interval + tacts; 
          c_index = nxt_index;
          nxt_index = ints[nxt_index].next_item;            
        }
        c_item = &ints[c_index];
        // uint8_t nxt_index = ints[c_index].next_item;
        // while ((nxt_index<255) && (ints[nxt_index].interval >= in_item->interval )) {// надо выбрать между какими из следующихх нам бы приткнуться->->-> это самый длинный процес сдесь            
        //   tacts = 0 - ints[nxt_index].interval;
        //   in_item->interval = in_item->interval + tacts; 
        //   c_index = nxt_index;
        //   nxt_index = ints[nxt_index].next_item;            
        // }

        if (in_item->interval == 0){
          in_item->interval = INTERRUPT_COUNTS;
          if (c_item->interval < INTERRUPT_COUNTS){
            c_item->interval++;
          }
        }
      }
    }
  


    uint8_t n_index = 255;
    if (c_index < 255){      
      n_index = c_item->next_item;
      c_item->next_item = index;
      
      // ints[c_index].tmp = (INTERRUPT_COUNTS-in_item->interval) ;
      // in_item->tmp = 3;
    }else{
      TCNT1 = in_item->interval; //if 
      
      n_index = current_item;
      // in_item->tmp = n_index;
      current_item = index;  
    }
    if (n_index < 255){
      tacts = ~in_item->interval;
      // ints[index].tmp = (INTERRUPT_COUNTS-in_item->interval);
      // ints[n_index].tmp = ints[n_index].interval;
      // ints[n_index].interval = (INTERRUPT_COUNTS-25);
      // if (ints[n_index].interval < in_item->interval){
      ints[n_index].interval = ints[n_index].interval + tacts;
      // }
      

    }
    in_item->next_item = n_index;



    // if (current_item<255){      
    //   if (TCNT1<in_item.interval){ // значит мы должны быть впереди....
    //     TCNT1 = in_item.interval;
    //     ints[current_item].interval = TCNT1;
    //     in_item.next_item = current_item;
    //     current_item = index;
    //   }else{
    //     interrupts_stek_item c_item = ints[current_item];        
    //     if (c_item.next_item<255) {
    //       //uint16_t last = (INTERRUPT_COUNTS - TCNT1);
    //       // interrupts_stek_item * c_item = ints[current_item];  
    //       in_item.interval = in_item.interval + (INTERRUPT_COUNTS - TCNT1); 
    //       while (c_item.next_item<255) {// надо выбрать между какими из следующихх нам бы приткнуться... это самый длинный процес сдесь            
    //         uint8_t nxt_item_index = c_item.next_item;  
    //         if (c_item.interval > in_item.interval ){
    //           in_item.interval = in_item.interval + (INTERRUPT_COUNTS - c_item.interval); 
    //           c_item = ints[nxt_item_index];
    //         }else{ // меньше либо равно сдесь. тоесть нам надо воткнуться перед c_item
    //           in_item.next_item = c_item.next_item ;
    //           c_item.next_item = index; 
    //           ints[nxt_item_index].interval = ints[nxt_item_index].interval + (INTERRUPT_COUNTS-in_item.interval);
    //           break;           
    //         }        
    //       }

    //     }else{
    //       c_item.next_item = index;
    //       in_item.interval = in_item.interval + (INTERRUPT_COUNTS - TCNT1); 
    //     }
        
               
    //   }
    // }else{
    //   in_item.next_item = 255;
    //   TCNT1 = in_item.interval; 
    //   current_item = index;       
    // }
}



cnt_class::cnt_class(){
  // launch_timers();
}

void cnt_class::launch_timers(){
  SREG = SREG | B10000000;  
  //Установка делителей таймера TCNT1H
  TCCR1A = B00000000;// Set Timer/Counter 1 in normal mode where // it will count to 0xFFFF then repeat.
  TIMSK1 = TIMSK1 | B00000001; // Turn on Timer/Counter 1 overflow // interrupt (bit 0).  
  // TCCR1B = TCCR1B | B00000001; // Turn on the counter with no prescaler.
  TCCR1B = 0x04;          // 16MHz clock with prescaler means TCNT1 increments every 4uS
  TCNT1 = 0;
  
    //Установка делителей таймера
  TCCR2A = 0;
  TCCR2B |= 1<<CS22;
  TCCR2B &= ~((1<<CS21) | (0<<CS20));
  TCCR2B &= ~((1<<WGM21) | (1<<WGM20));
  //Timer2 Overflow Interrupt Enable  
  TIMSK2 |= 1<<TOIE2; 
  //reset timer
  TCNT2 = 0;



}


